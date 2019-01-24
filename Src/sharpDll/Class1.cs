using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace sharpDll
{
    public class mathTest
    {
        public mathTest()
        {
            value1 =0;
            value2 = 0;
            valueAfter = 0;
        }
        public mathTest add(int a, int b)
        {
            value1 = a;
            value2 = b;
            valueAfter = a + b;
            return this;
        }

        public override string ToString()
        {
            return string.Format("{0}+{1}={2}", value1, value2, valueAfter);
        }
        private int value1 = 0;
        private int value2 = 0;
        private int valueAfter = 0;

    }
}
