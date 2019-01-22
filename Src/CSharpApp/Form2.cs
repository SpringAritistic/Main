using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CSharpApp
{
    public partial class Form2 : Form
    {

        public Form2()
        {
            InitializeComponent();
        }

        private void button_find_Click(object sender, EventArgs e)
        {
            MessageBox.Show("查找字符串");
        }

        private void button_replace_Click(object sender, EventArgs e)
        {
            MessageBox.Show("替换字符串");
        }
    }
}
