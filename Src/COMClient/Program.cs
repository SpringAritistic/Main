using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using MyServer;

namespace COMClient
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            var obj = new COMDemo();
            IWelcome welcome = obj;
            Console.WriteLine(welcome.Greeting("John"));
            //
            IMath math = (IMath)obj;
            int x = math.Add(3, 5);
            Console.WriteLine(x);
            //释放内存
            Marshal.ReleaseComObject(math);

        }
    }
}
