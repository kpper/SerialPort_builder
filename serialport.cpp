
#include <vcl.h>
#pragma hdrstop

#include "serialport.h"
// HANDLE comHandle = NULL;  //串口的句柄
//DCB dcb;                  //设备控制块
//---------------------------------------------------------------------------
#pragma package(smart_init)
__fastcall SerialPort::SerialPort()
{
    cout<<"串口类功能测试基于Win API"<<endl;
     comHandle=NULL;
}
__fastcall SerialPort::~SerialPort ()
{
   CloseCOM();
   cout<<"串口关闭"<<endl;

}
__fastcall SerialPort::SerialPort(AnsiString comm)
{
    //OpenCOM (comm);
    Open(comm);
   // cout<<"串口类功能测试基于Win API"<<endl;
    //
}
//打开串口
void SerialPort:: OpenCOM(AnsiString comm) {
        //CloseCOM();
    comHandle = CreateFile(
                comm.c_str(),                                   //指定要打开的串口
                GENERIC_READ | GENERIC_WRITE,                   //读写访问模式
                0,                                              //共享模式 由于串口不能共享必须为0
                NULL,                                           //引用安全性属性结构 缺省为NULL
                OPEN_EXISTING,                                  //打开串口必须这样设置
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,   //重叠操作方式又称异步操作方式 读写文件必须指定一个OVERLAPPED结构
                NULL                                            //对于串口必须为NULL
        );
    //ConfigCOM(9600);
     GetCommState(comHandle,&dcb);                     //得知目前串行端口的状态
  dcb.BaudRate = CBR_9600;
  dcb.ByteSize = 8;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  if(!SetCommState(comHandle,&dcb)) //设置通信端口的状态
  {
        MessageBox(0,"通信端口设置错误!!!","Set ERROR",MB_OK);
        CloseHandle(comHandle);
        return;
  }
    if (comHandle == INVALID_HANDLE_VALUE) {
        //MainForm->ShowMsg("打开" + comm + "失败", clRed);
        cout<<"打开"<<comm.c_str ()<<"失败"<<endl;
    }
    else
    {
         //   MainForm->ShowMsg("打开" + comm + "成功", clGreen);
         cout<<"打开"<<comm.c_str ()<<"成功"<<endl;
    }
}

void SerialPort::Open(AnsiString ComNo)
{
  DCB dcb;                          //转换至指针类型Char
  comHandle=CreateFile(ComNo.c_str(),GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,0);
  if(comHandle==INVALID_HANDLE_VALUE)
  {
        MessageBox(0,"打开通信端口错误!!","Comm Error",MB_OK);   //如果串行端口未打开
        return;
  }
  GetCommState(comHandle,&dcb);                     //得知目前串行端口的状态
  dcb.BaudRate = CBR_9600;
  dcb.ByteSize = 8;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  if(!SetCommState(comHandle,&dcb)) //设置通信端口的状态
  {
        MessageBox(0,"通信端口设置错误!!!","Set ERROR",MB_OK);
        CloseHandle(comHandle);
        return;
  }
}

//关闭串口
void SerialPort:: CloseCOM() {
    if (comHandle != NULL) {
        CloseHandle(comHandle);
                comHandle = NULL;
    }
}

//配置串口
void SerialPort:: ConfigCOM(unsigned long rate) {
        SetupComm(comHandle,1024,1024);    //设置输入输出缓冲区的大小都为1024个字节
        COMMTIMEOUTS timeOuts;//规定读写操作的超时
        //设定读超时
        timeOuts.ReadIntervalTimeout = 1000;
        timeOuts.ReadTotalTimeoutMultiplier = 500;
        timeOuts.ReadTotalTimeoutConstant = 500;
        //设定写超时
        timeOuts.WriteTotalTimeoutMultiplier = 500;
        timeOuts.WriteTotalTimeoutConstant = 2000;
        SetCommTimeouts(comHandle,&timeOuts);
        //获取串口的初始配置
        GetCommState(comHandle,&dcb);
        //修改串口的配置并保存
        dcb.BaudRate = rate;            //波特率
        dcb.ByteSize = 8;               //指定串口当前使用的数据位(4-8)
        dcb.fParity = false;            //是否允许奇偶校验 在为true的时候看Parity设置
        dcb.Parity = NOPARITY;          //校验方法(EVENPARITY,MARKPARITY,NOPARITY,ODDPARITY)
        dcb.StopBits = TWOSTOPBITS;     //停止位(ONESTOPBIT,ONE5STOPBITS,TWOSTOPBITS)
       // SetCommState(comHandle,&dcb);
        if(!SetCommState(comHandle,&dcb)) //设置通信端口的状态
         {
             //  MessageBox(0,"通信端口设置错误!!!","Set ERROR",MB_OK);
               cout<<"通信端口设置错误!!!"<<endl;
               CloseHandle(comHandle);
               return;
         }
        //读写串口之前必须清空缓冲区
        PurgeComm(comHandle,PURGE_TXCLEAR | PURGE_RXCLEAR);
}
//异步写串口数据
unsigned long SerialPort:: WriteCOM(char buffer[])
{
       // char buffer[3] = {0x16,0x54,0x0D};
        DWORD writeBytes = 1024;
        DWORD error;
        COMSTAT comStat;
        OVERLAPPED lapped;
        memset(&lapped,0,sizeof(OVERLAPPED));
        lapped.Offset = 0;
        lapped.OffsetHigh = 0;
        //lapped.hEvent事件设置为无信号状态
        lapped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
        //清除错误信息
        ClearCommError(comHandle,&error,&comStat);
        bool result = WriteFile(comHandle,buffer,writeBytes,&writeBytes,&lapped);
        if(!result)
        {
                //表示串口正在进行写取数据
                if(GetLastError() == ERROR_IO_PENDING)
                {
                        //等待2s 让读操作完成
                        WaitForSingleObject(lapped.hEvent,2000);
                        return writeBytes;
                }
                return 0;
        }
        return writeBytes;
}
//---------------------------------------------------------------------------
//异步读串口数据 返回实际读到的字节数
unsigned long SerialPort:: ReadCOM(char buffer[])
{
        DWORD readBytes = 1024;
        DWORD error;
        COMSTAT comStat;
        OVERLAPPED lapped;
        memset(&lapped,0,sizeof(OVERLAPPED));
        //lapped.hEvent事件设置为无信号状态
        lapped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
        //清除错误信息
        ClearCommError(comHandle,&error,&comStat);
        readBytes=min(readBytes,(DWORD)comStat.cbInQue);
        bool result = ReadFile(
                comHandle,
                buffer,
                readBytes,
                &readBytes,
                &lapped
        );
        if(!result)
        {
                //表示串口正在进行读取数据
                if(GetLastError() == ERROR_IO_PENDING)
                {
                        //等待2s 让读操作完成
                        WaitForSingleObject(lapped.hEvent,2000);
                        //当串口读操作完成后 lapped.hEvent事件会变为有信号状态
                        PurgeComm(comHandle,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
                        return readBytes;
                }
                return 0;
        }
        PurgeComm(comHandle,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
        return readBytes;
}

bool SerialPort:: Send(unsigned char * SendData,unsigned long t_size)                                   //发送数据
{
  bool SendFlag = false;
  unsigned long lrc;
  if(comHandle==0) return false;                            //检查Handle值
  SendFlag = WriteFile(comHandle,SendData,t_size,&lrc,NULL);      //送出数据
  return SendFlag;
}
//---------------------------------------------------------------------------
void SerialPort::Receiv(unsigned char * Data)                      //接收数据
{
  char inbuff[32]={0};
  DWORD nBytesRead,dwError;
  COMSTAT cs;
  ClearCommError(comHandle,&dwError,&cs);
  if(cs.cbInQue>sizeof(inbuff))                //取得状态
  {                                             //数据是否大于所准备的缓冲区
        PurgeComm(comHandle,PURGE_RXCLEAR);     //清除通信端口数据
        return;
  }
  ReadFile(comHandle,inbuff,cs.cbInQue,&nBytesRead,NULL);     //接收端口的数据
  memcpy(Data,inbuff,32);
}

