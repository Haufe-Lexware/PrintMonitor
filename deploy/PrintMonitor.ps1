# Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy Unrestricted
 param ([switch]$InstallPrinter = $true)

$PRINTERLOG = "Haufe Printer"
$MODULENAME = "haufe.components.printer"
$MODULEPATH = [System.IO.Path]::GetFullPath("$PSScriptRoot\..\..\..\..\build\bin\Debug\x64\$MODULENAME")
$SYSTEM32 = [System.Environment]::ExpandEnvironmentVariables("%SystemRoot%") + "\system32"
$THIRDPARTY = [System.IO.Path]::GetFullPath("$PSScriptRoot\..\..\..\..\ThirdParty")
$LEXWARE = [System.Environment]::ExpandEnvironmentVariables("%CommonProgramFiles(x86)%") + "\Lexware"
$GHOSTSCRIPT = "$LEXWARE\Ghostscript"
$INF = "$THIRDPARTY\Ghostscript\lib\ghostpdf.inf"
$BIN = "$THIRDPARTY\Ghostscript\bin"

function IsAdmin {
	([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
}

function RunElevated {
	Start-Process powershell -ArgumentList "-file `"$PSCommandPath`"" -verb RunAs 
}

function DeployModule($path) {
	Write-Host copy $path
    Copy-Item $path -Destination $SYSTEM32 -Force
}

function DeployModules {
	DeployModule "$MODULEPATH.dll"
	DeployModule "$MODULEPATH.pdb"
	if (-Not (Test-Path $GHOSTSCRIPT)) {
		if (-Not (Test-Path $LEXWARE)) {
			New-Item -Path $LEXWARE -ItemType directory
		}
		New-Item -Path $GHOSTSCRIPT -ItemType SymbolicLink -Value $BIN
	}
}

function RunDll($arguments) {
	Write-Host $arguments
	Start-Process -FilePath "rundll32.exe" -NoNewWindow -Wait -ArgumentList $arguments
}

function InstallPrinter() {
	if (Test-Path $INF) {
		RunDll "$SYSTEM32\$MODULENAME.dll,Install `"$INF`""
	} else {
		Write-Host error: `"$INF`" does not exist!
	}
}

function UninstallPrinter {
	RunDll "$SYSTEM32\$MODULENAME.dll,Uninstall"
}

if (IsAdmin) {
	if ($InstallPrinter) { 
		try { Clear-EventLog -LogName $PRINTERLOG -ErrorAction SilentlyContinue } catch { }
		UninstallPrinter
	}

	& net stop spooler
	DeployModules			# copy the modules to system32
	& net start spooler

	if ($InstallPrinter) {
		InstallPrinter
		try { Get-EventLog -LogName $PRINTERLOG -ErrorAction SilentlyContinue } catch { }
	}
	Pause
} else {
    RunElevated
}
