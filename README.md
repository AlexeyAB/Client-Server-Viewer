Client-Server-Viewer
====================

Simple example of the Client-Server Viewer for large files with Boost.Asio and MFC

Contains two project: 
- server-side: ASIO_SERVER.vcxproj
- client-side: MFC_UI.vcxproj

Can be compiled:
- by using MSVS2010(VC10/_MSC_VER == 1600) and MSVS2012(VC11/_MSC_VER == 1700)
- requires library Boost >= 1.53

Example of use ASIO_SERVER.exe from Windows console:
- Usage: asio_server.exe [file_name local_port local_address timeout_sec number_acceptors numebr_executors language_locale]
- Default: asio_server.exe  list.txt 10001 0.0.0.0 120 2 4 Russian_Russia.1251

For using client MFC_UI.exe - simply start it and use its GUI:
- to set options
- to connect to the server of viewer
- to view and navigate over remote file

**Features:**

On the server-side, uses: 
- asynchronous operations
- setup timeout for asynchronous operations
- optimizations: thread pools, memory pools and custom allocators handlers
- the ability to error logging to the console or to a file, by your choosing

On the client-side, uses: 
- very simple GUI only for presentation of network communication capabilities
- part of operations in blocking and non-blocking asynchronous mode
- setup the server address, port and timeout for some communication operations
- navigating over remote file by using only a single ScrollBar.  

Tested and works with very large file - 100GB in Windows7x64.



**Attention!** Bug: incorrect getting size of file large than 4 GB, bug fixed since from MSVS2012(VC11/_MSC_VER == 1700)
http://connect.microsoft.com/VisualStudio/feedback/details/627639/std-fstream-use-32-bit-int-as-pos-type-even-on-x64-platform

