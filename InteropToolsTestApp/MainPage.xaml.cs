using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics;
using Windows.Graphics.Capture;
using Windows.Graphics.DirectX;
using Windows.Graphics.DirectX.Direct3D11;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.System;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using WinRTInteropTools;

namespace InteropToolsTestApp
{
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();

            Init();
        }

        private void Init()
        {
            _compositor = Window.Current.Compositor;
            _device = new Direct3D11Device();
            _deviceContext = _device.ImmediateContext;
            _multithread = _device.Multithread;
            _multithread.IsMultithreadProtected = true;
            _decoder = new ImageDecoder();
            _compositionGraphicsDevice = CompositionGraphics.CreateCompositionGraphicsDevice(_compositor, _device);

            _root = _compositor.CreateContainerVisual();
            _root.RelativeSizeAdjustment = Vector2.One;
            ElementCompositionPreview.SetElementChildVisual(this, _root);

            _imageSurface = _compositionGraphicsDevice.CreateDrawingSurface(
                new Size(1, 1), 
                DirectXPixelFormat.B8G8R8A8UIntNormalized, 
                DirectXAlphaMode.Premultiplied);

            _imageBrush = _compositor.CreateSurfaceBrush(_imageSurface);
            _imageBrush.HorizontalAlignmentRatio = 0;
            _imageBrush.VerticalAlignmentRatio = 0;
            _imageBrush.Stretch = CompositionStretch.None;
            _imageBrush.BitmapInterpolationMode = CompositionBitmapInterpolationMode.NearestNeighbor;

            _imageVisual = _compositor.CreateSpriteVisual();
            _imageVisual.RelativeSizeAdjustment = Vector2.One;
            _imageVisual.Brush = _imageBrush;
            _root.Children.InsertAtTop(_imageVisual);

            _captureBrush = _compositor.CreateSurfaceBrush();

            _captureVisual = _compositor.CreateSpriteVisual();
            _captureVisual.RelativeSizeAdjustment = Vector2.One;
            _captureVisual.Brush = _captureBrush;
            _root.Children.InsertAtBottom(_captureVisual);

            _swapChain = new SwapChain(
                _device,
                DirectXPixelFormat.B8G8R8A8UIntNormalized,
                2,
                new SizeInt32() { Width = 1, Height = 1 });
            var surface = _swapChain.CreateSurface(_compositor);
            _captureBrush.Surface = surface;
        }

        private async Task<StorageFile> PickImageAsync()
        {
            var picker = new FileOpenPicker();
            picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            picker.ViewMode = PickerViewMode.Thumbnail;
            picker.FileTypeFilter.Add(".jpg");
            picker.FileTypeFilter.Add(".png");

            var file = await picker.PickSingleFileAsync();
            return file;
        }

        private async Task<StorageFile> PickVideoAsync()
        {
            var picker = new FileSavePicker();
            picker.SuggestedStartLocation = PickerLocationId.VideosLibrary;
            picker.SuggestedFileName = "encodedVideo";
            picker.DefaultFileExtension = ".mp4";
            picker.FileTypeChoices.Add("MP4 Video", new List<string> { ".mp4" });

            var file = await picker.PickSaveFileAsync();
            return file;
        }

        private void StopCapture()
        {
            if (_framePool != null)
            {
                _framePool.FrameArrived -= OnFrameArrived;
                _framePool.Dispose();
                _framePool = null;
            }
            _session?.Dispose();
            if (_item != null)
            {
                _item.Closed -= OnCaptureItemClosed;
                _item = null;
            }
        }

        private void StartCapture(GraphicsCaptureItem item)
        {
            _framePool = Direct3D11CaptureFramePool.CreateFreeThreaded(
                _device,
                DirectXPixelFormat.B8G8R8A8UIntNormalized,
                2,
                item.Size);
            _session = _framePool.CreateCaptureSession(item);
            _framePool.FrameArrived += OnFrameArrived;
            _session.StartCapture();
            _item = item;
            _item.Closed += OnCaptureItemClosed;
        }

        private void OnFrameArrived(Direct3D11CaptureFramePool sender, object args)
        {
            var newSize = false;

            using (var frame = sender.TryGetNextFrame())
            {
                if (frame.ContentSize.Width != _lastSize.Width ||
                    frame.ContentSize.Height != _lastSize.Height)
                {
                    newSize = true;
                    _lastSize = frame.ContentSize;
                    _swapChain.Resize(
                        DirectXPixelFormat.B8G8R8A8UIntNormalized, 
                        2, 
                        _lastSize);
                }

                using (var backBuffer = _swapChain.GetBuffer(0))
                using (var lockSession = _multithread.Lock())
                {
                    _deviceContext.CopyResource(backBuffer, frame.Surface);
                }
            }

            _swapChain.Present();   

            if (newSize)
            {
                _framePool.Recreate(
                    _device,
                    DirectXPixelFormat.B8G8R8A8UIntNormalized,
                    2,
                    _lastSize);
            }
        }

        private void OnCaptureItemClosed(GraphicsCaptureItem sender, object args)
        {
            StopCapture();
        }

        private async void CaptureButton_Click(object sender, RoutedEventArgs e)
        {
            StopCapture();

            var picker = new GraphicsCapturePicker();
            var item = await picker.PickSingleItemAsync();
            if (item != null)
            {
                StartCapture(item);
            }
        }

        private async void ImageButton_Click(object sender, RoutedEventArgs e)
        {
            _imageBrush.Surface = null;

            var file = await PickImageAsync();
            if (file != null)
            {
                using (var stream = await file.OpenReadAsync())
                using (var d3dSurface = _decoder.DecodeStreamIntoTexture(_device, stream))
                {
                    CompositionGraphics.CopyDirect3DSurfaceIntoCompositionSurface(_device, d3dSurface, _imageSurface);
                }
                _imageBrush.Surface = _imageSurface;
            }
        }

        private async void RecordButton_Click(object sender, RoutedEventArgs e)
        {
            var button = (AppBarToggleButton)sender;
            if (button.IsChecked.Value)
            {
                var picker = new GraphicsCapturePicker();
                var item = await picker.PickSingleItemAsync();
                if (item == null)
                {
                    button.IsChecked = false;
                    return;
                }

                var file = await PickVideoAsync();
                if (file == null)
                {
                    button.IsChecked = false;
                    return;
                }

                using (var stream = await file.OpenAsync(FileAccessMode.ReadWrite))
                using (_encoder = new Encoder(_device, item))
                {
                    await _encoder.EncodeAsync(stream);
                }
            }
            else
            {
                _encoder?.Dispose();
            }
        }

        private async void ScreenshotButton_Click(object sender, RoutedEventArgs e)
        {
            var filePicker = new FileSavePicker();
            filePicker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            filePicker.SuggestedFileName = "screenshot";
            filePicker.DefaultFileExtension = ".png";
            filePicker.FileTypeChoices.Add("PNG Image", new string[] { ".png" });
            var file = await filePicker.PickSaveFileAsync();

            if (file != null)
            {
                var capturePicker = new GraphicsCapturePicker();
                var item = await capturePicker.PickSingleItemAsync();

                if (item != null)
                {
                    var framePool = Direct3D11CaptureFramePool.CreateFreeThreaded(
                        _device,
                        DirectXPixelFormat.B8G8R8A8UIntNormalized,
                        1,
                        item.Size);
                    var session = framePool.CreateCaptureSession(item);

                    var completionSource = new TaskCompletionSource<Direct3D11Texture2D>();
                    framePool.FrameArrived += (s, a) =>
                    {
                        using (var frame = s.TryGetNextFrame())
                        {
                            var frameTexture = Direct3D11Texture2D.CreateFromDirect3DSurface(frame.Surface);
                            var description = frameTexture.Description2D;
                            description.Usage = Direct3DUsage.Staging;
                            description.BindFlags = 0;
                            description.CpuAccessFlags = Direct3D11CpuAccessFlag.AccessRead;
                            description.MiscFlags = 0;
                            var copyTexture = _device.CreateTexture2D(description);

                            _deviceContext.CopyResource(copyTexture, frameTexture);

                            session.Dispose();
                            framePool.Dispose();

                            completionSource.SetResult(copyTexture);
                        }
                    };

                    session.StartCapture();
                    var texture = await completionSource.Task;
                    var bits = texture.GetBytes();

                    using (var stream = await file.OpenAsync(FileAccessMode.ReadWrite))
                    {
                        var encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.PngEncoderId, stream);
                        encoder.SetPixelData(
                            BitmapPixelFormat.Bgra8,
                            BitmapAlphaMode.Premultiplied,
                            (uint)item.Size.Width,
                            (uint)item.Size.Height,
                            1.0,
                            1.0,
                            bits);
                        await encoder.FlushAsync();
                    }

                    await Launcher.LaunchFileAsync(file);
                }
            }
        }

        private Compositor _compositor;
        private CompositionGraphicsDevice _compositionGraphicsDevice;
        private Direct3D11Device _device;
        private Direct3D11DeviceContext _deviceContext;
        private Direct3D11Multithread _multithread;
        private ImageDecoder _decoder;
        private SwapChain _swapChain;

        private ContainerVisual _root;
        private SpriteVisual _imageVisual;
        private SpriteVisual _captureVisual;
        private CompositionDrawingSurface _imageSurface;
        private CompositionSurfaceBrush _imageBrush;
        private CompositionSurfaceBrush _captureBrush;

        private GraphicsCaptureItem _item;
        private GraphicsCaptureSession _session;
        private Direct3D11CaptureFramePool _framePool;
        private SizeInt32 _lastSize;

        private Encoder _encoder;
    }
}
