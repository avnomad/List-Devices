// disable deprecation warnings
#pragma warning(disable : 4996)

// compilation control defines
#if defined(_DEBUG) && !defined(DEBUG)
#define DEBUG
#endif

//#define UNICODE

// includes
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::clog;
using std::left;

#include <iomanip>
using std::setw;
using std::setprecision;

#include <fstream>
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::ios;

#include <iterator>
using std::istream_iterator;
using std::ostream_iterator;
using std::back_inserter;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <sstream>
using std::ostringstream;
using std::istringstream;
using std::stringstream;

#include <cstdlib>
using std::system;
using std::exit;

#include <cstring>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


#include "../../Unsorted/algorithms.h"
#include "../../Unsorted/windows common.h"
#include "../../Unsorted/char reader.h"

#include <utility>


BOOL __stdcall DIEnumDevicesCallback(LPCDIDEVICEINSTANCE device , VOID *devices)
{
	((vector<DIDEVICEINSTANCE> *)devices)->push_back(*device);
	return DIENUM_CONTINUE;
} // end function DIEnumDevicesCallback


BOOL __stdcall DIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE button_or_axis, VOID *stream)
{
	*(ofstream*)stream << "\t\t\t\t\t<tr>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << read_guid_type(button_or_axis->guidType) << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->dwOfs << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << read_control_type(button_or_axis->dwType) << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << read_data_format_flags(button_or_axis->dwFlags) << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->tszName << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->dwFFMaxForce << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->dwFFForceResolution << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->wCollectionNumber << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->wDesignatorIndex << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->wUsagePage << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->wUsage << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->dwDimension << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->wExponent << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t\t<td>" << button_or_axis->wReportId << "</td>\n";
	*(ofstream*)stream << "\t\t\t\t\t</tr>\n";

	return DIENUM_CONTINUE;
} // end function DIEnumDeviceObjectsCallback


int WINAPI WinMain(InstanceHandle currentInstance , InstanceHandle PreviusInstance , CHAR *commandLineArguments , int windowMode)
{
	IDirectInput8 *directInputObject;
	HRESULT hr = DirectInput8Create(currentInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&directInputObject,NULL);


	// open and check output stream
	ofstream output("c:/output/devices.html");

	if(!output)
	{
		char temp[MAX_PATH] = _T("Cannot open file:\n\t");
		strcat(temp,"C:/output/devices.html");
		strcat(temp,"\nfor writing.");
		MessageBox(0,temp,_T("An error has occured!"),MB_OK | MB_ICONERROR);
		directInputObject->Release();
		exit(0);
	} // end if


	// open and check input stream
	ifstream input("template.html");

	if(!input)
	{
		char temp[MAX_PATH] = _T("Cannot open file:\n\t");
		strcat(temp,"template.html");
		strcat(temp,"\nfor reading.");
		MessageBox(0,temp,_T("An error has occured!"),MB_OK | MB_ICONERROR);
		directInputObject->Release();
		exit(0);
	} // end if

	// create a string with the desired sequence
	string marker("<!--|-->\n");
	// create some strings to hold pieces of the template that will be repeatedly used.
	string open_first_table , close_first_and_open_second_table , close_second_table;

	// create a triple with the iterators and a bool value
	// (this will be used to store the result of copy_until_sequence)
	Triple<CharReader,ostream_iterator<char>,bool>  positions(CharReader(input),ostream_iterator<char>(output),false);


	// copy prologue of the html file from template
	positions = copy_until_sequence(positions.first,CharReader(),positions.second,marker.begin(),marker.end());
	// get string to open first table from template
	positions.first = copy_until_sequence(positions.first,CharReader(),back_inserter(open_first_table),marker.begin(),marker.end()).first;
	// get string to close first table and open the second from template
	positions.first = copy_until_sequence(positions.first,CharReader(),back_inserter(close_first_and_open_second_table),marker.begin(),marker.end()).first;
	// get string to close second table from template
	positions.first = copy_until_sequence(positions.first,CharReader(),back_inserter(close_second_table),marker.begin(),marker.end()).first;


	// create a vector to store the enumerated devices
	vector<DIDEVICEINSTANCE> devices;

	// enumerate system devices
	directInputObject->EnumDevices(DI8DEVCLASS_ALL,DIEnumDevicesCallback,(void *)&devices,DIEDFL_ALLDEVICES);
	for(size_t c = 0 ; c < devices.size() ; ++c)	// Note that this part writes to the output stream without changing the ostream_iterator. Thankfully it still works.
	{
		output << open_first_table;
		output << "\t\t\t\t\t<tr>\n";
		output << "\t\t\t\t\t\t<td>" << devices[c].tszInstanceName << "</td>\n";
		output << "\t\t\t\t\t\t<td>" << devices[c].tszProductName << "</td>\n";
		output << "\t\t\t\t\t\t<td>" << devices[c].guidInstance << "</td>\n";
		output << "\t\t\t\t\t\t<td>" << devices[c].guidProduct << "</td>\n";
		output << "\t\t\t\t\t\t<td>" << devices[c].guidFFDriver << "</td>\n";
		output << "\t\t\t\t\t\t<td>" << read_device_type(devices[c].dwDevType & 0x000000ff) << "</td>\n";
		output << "\t\t\t\t\t\t<td>" << read_device_subtype(devices[c].dwDevType & 0x0000ffff) << "</td>\n";
		output << "\t\t\t\t\t\t<td>" << devices[c].wUsagePage << "</td>\n";
		output << "\t\t\t\t\t\t<td>" << devices[c].wUsage << "</td>\n";	
		output << "\t\t\t\t\t</tr>\n";
		output << close_first_and_open_second_table;

		IDirectInputDevice8 *device;	// a pointer to a device (mouse, keyboard, ect.)
		if(directInputObject->CreateDevice(devices[c].guidInstance,&device,NULL) != DI_OK)
		{
			MessageBox(0,("Cannot create the device \""+string(devices[c].tszInstanceName)+"\".").c_str(),_T("An error has occured!"),MB_OK | MB_ICONERROR);
		} // end if
		else
		{
			device->EnumObjects(DIEnumDeviceObjectsCallback,(void *)&output,DIDFT_ALL);
			device->Release();
		} // end if-else

		output << close_second_table;
	} // end for


	// copy epilogue of the html file from template
	positions = copy_until_sequence(positions.first,CharReader(),positions.second,marker.begin(),marker.end());


	// close the output file so that another program can open it
	output.close();

	// call the default program to open the output file and display the results
	if(system(0))
	{
		system("start c:/output/devices.html");
	}
	else
		MessageBox(0,_T("No command interpreter is present!"),_T("Warning!"),MB_OK | MB_ICONWARNING);

#ifndef DEBUG
	// display some information to the user.
	MessageBox(0,_T("Output was saved in c:/output/devices.html."),_T("Note:"),MB_OK | MB_ICONINFORMATION);
#endif
	directInputObject->Release();
} // end function WinMain
