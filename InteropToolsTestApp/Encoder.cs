using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Graphics.Capture;
using Windows.Graphics.DirectX;
using Windows.Graphics.DirectX.Direct3D11;
using Windows.Media.Core;
using Windows.Media.MediaProperties;
using Windows.Media.Transcoding;
using Windows.Storage.Streams;
using WinRTInteropTools;

namespace InteropToolsTestApp
{
    class Encoder : IDisposable
    {
        public Encoder(Direct3D11Device device, GraphicsCaptureItem item)
        {
            _device = device;
            _deviceContext = device.ImmediateContext;
            _multiThread = _device.Multithread;
            _captureItem = item;
            _captureItem.Closed += OnCaptureItemClosed;
            _isRecording = false;

            CreateMediaObjects();
        }

        public async Task EncodeAsync(IRandomAccessStream stream)
        {
            if (!_isRecording)
            {
                _isRecording = true;

                // Initialize our capture objects
                _framePool = Direct3D11CaptureFramePool.Create(
                    _device,
                    DirectXPixelFormat.B8G8R8A8UIntNormalized,
                    1,
                    _captureItem.Size);
                _captureSession = _framePool.CreateCaptureSession(_captureItem);

                // Start the capture
                _captureSession.StartCapture();

                try
                {
                    var encodingProfile = new MediaEncodingProfile();
                    encodingProfile.Container.Subtype = "MPEG4";
                    encodingProfile.Video.Subtype = "H264";
                    encodingProfile.Video.Width = (uint)_captureItem.Size.Width;
                    encodingProfile.Video.Height = (uint)_captureItem.Size.Height;
                    encodingProfile.Video.Bitrate = _videoDescriptor.EncodingProperties.Bitrate;
                    encodingProfile.Video.FrameRate.Numerator = c_frameRateN;
                    encodingProfile.Video.FrameRate.Denominator = 1;
                    encodingProfile.Video.PixelAspectRatio.Numerator = 1;
                    encodingProfile.Video.PixelAspectRatio.Denominator = 1;
                    var transcode = await _transcoder.PrepareMediaStreamSourceTranscodeAsync(_mediaStreamSource, stream, encodingProfile);

                    await transcode.TranscodeAsync();
                }
                catch (Exception ex)
                {
                    System.Diagnostics.Debug.WriteLine(ex.Message);
                    System.Diagnostics.Debug.WriteLine(ex);
                }
            }
        }

        public void Dispose()
        {
            _isRecording = false;
            _device.Dispose();
            _deviceContext.Dispose();
            _multiThread.Dispose();
            _framePool.Dispose();
            _captureSession.Dispose();
            _captureItem.Closed -= OnCaptureItemClosed;
        }

        private void OnCaptureItemClosed(GraphicsCaptureItem sender, object args)
        {
            // This is a nasty trick
            _isRecording = !_isRecording;
        }

        private void CreateMediaObjects()
        {
            // Create our encoding profile based on the size of the item
            // TODO: This only really makes sense for monitors, we need
            //       to change this to make sense in all cases.
            int width = _captureItem.Size.Width;
            int height = _captureItem.Size.Height;

            // Describe our input: uncompressed BGRA8 buffers comming in at the monitor's refresh rate
            // TODO: We pick 60Hz here because it applies to most monitors. However this should be
            //       more robust.
            var videoProperties = VideoEncodingProperties.CreateUncompressed(MediaEncodingSubtypes.Bgra8, (uint)width, (uint)height);
            _videoDescriptor = new VideoStreamDescriptor(videoProperties);
            _videoDescriptor.EncodingProperties.FrameRate.Numerator = c_frameRateN;
            _videoDescriptor.EncodingProperties.FrameRate.Denominator = c_frameRateD;
            _videoDescriptor.EncodingProperties.Bitrate = (uint)(c_frameRateN * c_frameRateD * width * height * 4);

            // Create our MediaStreamSource
            _mediaStreamSource = new MediaStreamSource(_videoDescriptor);
            _mediaStreamSource.BufferTime = TimeSpan.FromSeconds(0);
            _mediaStreamSource.Starting += OnMediaStreamSourceStarting;
            _mediaStreamSource.SampleRequested += OnMediaStreamSourceSampleRequested;

            // Create our device manager
            _mediaGraphicsDevice = MediaGraphicsDevice.CreateFromMediaStreamSource(_mediaStreamSource);
            _mediaGraphicsDevice.RenderingDevice = _device;

            // Create our transcoder
            _transcoder = new MediaTranscoder();
            _transcoder.HardwareAccelerationEnabled = true;
        }

        private void OnMediaStreamSourceSampleRequested(MediaStreamSource sender, MediaStreamSourceSampleRequestedEventArgs args)
        {
            if (_isRecording)
            {
                using (var frame = GetNextFrame())
                using (var lockSession = _multiThread.Lock())
                using (var sourceTexture = Direct3D11Texture2D.CreateFromDirect3DSurface(frame.Surface))
                using (var surface = _device.CreateTexture2D(new Direct3D11Texture2DDescription()
                {
                    Base = sourceTexture.Description2D.Base,
                    MipLevels = sourceTexture.Description2D.MipLevels,
                    ArraySize = sourceTexture.Description2D.ArraySize,
                    Usage = Direct3DUsage.Default,
                    BindFlags = Direct3DBindings.ShaderResource,
                    CpuAccessFlags = 0,
                    MiscFlags = 0
                }))
                {
                    var timeStamp = frame.SystemRelativeTime;
                    _deviceContext.CopyResource(surface, frame.Surface);

                    try
                    {
                        var sample = MediaStreamSample.CreateFromDirect3D11Surface(surface, timeStamp);
                        args.Request.Sample = sample;
                    }
                    catch (Exception e)
                    {
                        System.Diagnostics.Debug.WriteLine(e);
                        args.Request.Sample = null;
                    }
                }
            }
            else
            {
                args.Request.Sample = null;
            }
        }

        private void OnMediaStreamSourceStarting(MediaStreamSource sender, MediaStreamSourceStartingEventArgs args)
        {
            DrainFramePool();
            using (var frame = GetNextFrame())
            {
                args.Request.SetActualStartPosition(frame.SystemRelativeTime);
            }
        }

        private Direct3D11CaptureFrame GetNextFrame()
        {
            Direct3D11CaptureFrame frame = null;

            do
            {
                frame = _framePool.TryGetNextFrame();
            } while (frame == null);

            return frame;
        }

        private void DrainFramePool()
        {
            for (var frame = GetNextFrame(); frame != null; frame = _framePool.TryGetNextFrame())
            {
                frame.Dispose();
            }
        }

        private Direct3D11Device _device;
        private Direct3D11DeviceContext _deviceContext;
        private Direct3D11Multithread _multiThread;
        private MediaGraphicsDevice _mediaGraphicsDevice;

        private Direct3D11CaptureFramePool _framePool;
        private GraphicsCaptureSession _captureSession;
        private GraphicsCaptureItem _captureItem;

        private VideoStreamDescriptor _videoDescriptor;
        private MediaStreamSource _mediaStreamSource;
        private MediaTranscoder _transcoder;
        private bool _isRecording;

        // Constants
        private const int c_frameRateN = 60;
        private const int c_frameRateD = 1;
    }
}
