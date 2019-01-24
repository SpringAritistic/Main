using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace Frame
{
	public partial class MainPage : UserControl
	{
		public MainPage()
		{
			// Required to initialize variables
			InitializeComponent();
		}

		private void myClick(object sender, System.Windows.RoutedEventArgs e)
		{
			// TODO: Add event handler implementation here.
			if(this.Test.Content.ToString()=="Hello")
			{
			this.Test.Content="Kitty";
			this.Test.Background = new SolidColorBrush(Colors.Blue);
			}
			else
			{
			this.Test.Content="Hello";
			this.Test.Background = new SolidColorBrush(Colors.Red);
			}

		}
	}
}