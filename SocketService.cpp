//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <iostream>
#include "serialport.h"
 using namespace std;
//---------------------------------------------------------------------------

#pragma argsused
SerialPort *Com;
//TCOM *Com;
int main(int argc, char* argv[])
{

        cout<<"HELLO"<<endl;
        String Str = "";
        String sendStr = "บวบว";
        String sendStr1 = "1234";
        unsigned char data[100];
        Com=new SerialPort("COM5");
        //Com=new TCOM("COM6");
        Com->Send(sendStr.c_str(),sendStr.Length());
         Sleep(100);
         Com->Receiv(data);
         Str = (char *)data;
        cout<<Str.c_str()<<endl;

        Com->WriteCOM(sendStr1.c_str());
         Sleep(100);
        Com->ReadCOM(data);
         Str = (char *)data;
        cout<<Str.c_str()<<endl;
        system("pause");
        return 0;
}
//---------------------------------------------------------------------------
 
