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
using Windows.Storage;
using Windows.Storage.Pickers;
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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace InteropToolsTestApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
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
            _deviceContext = _device.GetImmediateContext();
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
            _framePool = Direct3D11CaptureFramePool.Create(
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
            else
            {
                _imageBrush.Surface = null;
            }
        }

        private Compositor _compositor;
        private CompositionGraphicsDevice _compositionGraphicsDevice;
        private Direct3D11Device _device;
        private Direct3D11DeviceContext _deviceContext;
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
    }
}
