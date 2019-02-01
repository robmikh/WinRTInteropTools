using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
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

            var ignored = InitAsync();
        }

        private async Task InitAsync()
        {
            _compositor = Window.Current.Compositor;
            _device = new Direct3D11Device();
            _decoder = new ImageDecoder();
            _compositionGraphicsDevice = CompositionGraphics.CreateCompositionGraphicsDevice(_compositor, _device);

            _surface = _compositionGraphicsDevice.CreateDrawingSurface(
                new Size(1, 1), 
                DirectXPixelFormat.B8G8R8A8UIntNormalized, 
                DirectXAlphaMode.Premultiplied);

            var brush = _compositor.CreateSurfaceBrush(_surface);
            brush.HorizontalAlignmentRatio = 0;
            brush.VerticalAlignmentRatio = 0;
            brush.Stretch = CompositionStretch.None;
            brush.BitmapInterpolationMode = CompositionBitmapInterpolationMode.NearestNeighbor;

            _visual = _compositor.CreateSpriteVisual();
            _visual.RelativeSizeAdjustment = Vector2.One;
            _visual.Brush = brush;
            ElementCompositionPreview.SetElementChildVisual(this, _visual);

            var file = await PickImageAsync();
            if (file != null)
            {
                using (var stream = await file.OpenReadAsync())
                using (var d3dSurface = _decoder.DecodeStreamIntoTexture(_device, stream))
                {
                    CompositionGraphics.CopyDirect3DSurfaceIntoCompositionSurface(_device, d3dSurface, _surface);
                }
            }
        }

        private async Task<StorageFile> PickImageAsync()
        {
            var picker = new FileOpenPicker();
            picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            picker.ViewMode = PickerViewMode.Thumbnail;
            picker.FileTypeFilter.Add(".jpg");

            var file = await picker.PickSingleFileAsync();
            return file;
        }

        private Compositor _compositor;
        private CompositionGraphicsDevice _compositionGraphicsDevice;
        private Direct3D11Device _device;
        private ImageDecoder _decoder;

        private SpriteVisual _visual;
        private CompositionDrawingSurface _surface;
    }
}
