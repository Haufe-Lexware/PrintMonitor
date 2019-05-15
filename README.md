# PrintMonitor

This is a [Print Monitor](https://docs.microsoft.com/en-us/windows-hardware/drivers/print/writing-a-print-monitor) 
for the [Ghostscript](https://www.ghostscript.com/) printer driver.

To compile the project with [Visual Studio Community](https://visualstudio.microsoft.com/vs/community) the file 
* *haufe.components.printer.props.template*  

must be adapted and renamed to  
* *haufe.components.printer.props*.

The [Windows Driver Kit (WDK)](https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk) is also required (*dependency to winspool.lib*).

The *Print Monitor* can be installed with the *./deploy/PrintMonitor.ps1* Powershell script.