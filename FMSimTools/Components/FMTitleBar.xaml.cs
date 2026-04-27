using System.Windows;
using System.Windows.Controls;

namespace FMSimTools.Components
{
    /// <summary>
    /// Interaction logic for FMTitleBar.xaml
    /// </summary>
    public partial class FMTitleBar : UserControl
    {
        public string Title
        {
            get { return (string)GetValue(TitleProperty); }
            set { SetValue(TitleProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Title.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty TitleProperty =
            DependencyProperty.Register("Title", typeof(string), typeof(FMTitleBar), new PropertyMetadata(string.Empty));

        public FMTitleBar()
        {
            InitializeComponent();
        }
    }
}
