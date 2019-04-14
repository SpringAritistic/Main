using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Text;
using System.IO;

namespace CSharpApp
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {


            //byte[] key = { 0xAC, 0xEC, 0xDF, 0x57 };
            //using (var input = new FileStream(@"D:\MP3\庄心妍 - 再遇不到你这样的人.mp3", FileMode.Open, FileAccess.Read))
            //{
            //    var output = File.OpenWrite(@"E:\KuGou\Temp\test.mp3");
            //    //输出文件
            //    input.Seek(1024, SeekOrigin.Begin);
            //    //跳过1024字节的包头
            //    byte[] buffer = new byte[key.Length];  
            //    int length;
            //    while ((length = input.Read(buffer, 0, buffer.Length)) > 0)
            //    {
            //        for (int i = 0; i < length; i++)
            //        {
            //            var k = key[i];
            //            var kh = k >> 4;
            //            var kl = k & 0xf;
            //            var b = buffer[i];
            //            var low = b & 0xf ^ kl;//解密后的低4位
            //            var high = (b >> 4) ^ kh ^ low & 0xf;
            //            //解密后的高4位
            //            buffer[i] = (byte)(high << 4 | low);
            //        }
            //        output.Write(buffer, 0, length);
            //    } output.Close();
            //}
            //Console.WriteLine("按任意键退出...");
            //Console.ReadKey();


            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
    }
}
