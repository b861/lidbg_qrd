#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..//common//usb.h"
#include "..//common//my_type.h"
#include "..//common//debug.h"
#include "..//common//CamEnum.h"
#include "..//BurnMgr//BurnMgr.h"
#include "../../inc/lidbg_servicer.h"

#define	STR_INTRO		"Program : SONiX F/W Update Tool on Linux"
#define	STR_VERSION		"Version : v1.0.5.8" // yiling 2015/08/21 modify
#define	STR_DATE		"Date    : 2015/08/21" // yiling 2015/08/21 modify

#define	FILE_INI		"/storage/udisk/update.src"
#define CNTNUM			10

static const int id_num = 8;	// shawn 2009/11/03 for 232 and 275
ID_TABLE_S	id_table[id_num] = 
{
	{0x0c45, 0x62c0},
	{0x0c45, 0x62d0},
	{0x0c45, 0x62e0},
	{0x0c45, 0x62f0},
	{0x0c45, 0x6300},
	{0x0c45, 0x6310},
	{0x0c45, 0x6320},	// shawn 2009/11/03 add 232
	{0x0c45, 0x6350}	// shawn 2009/11/03 add 275
};

int nFileNum = 0;	// shawn 2010/11/12 add for fixing "No such device" issue


void Print_CamArray(struct usb_device* CamArray[MAX_CAM_NUM], int &nCamNum);
bool Burn_To_Flash(CBurnMgr	&burn_mgr);
bool Burn_To_File(CBurnMgr	&burn_mgr, char szTarget[256]);

void Print_CamArray(struct usb_device* CamArray[MAX_CAM_NUM], int &nCamNum)
{
	struct usb_device *pDev;
	struct usb_device_descriptor *des;
	usb_dev_handle *udev;
	int		i;
	int		vid, pid;
	char	szMfg[64];
	
	printf("# of webcam found = %d\n", nCamNum);
	for (i=0; i<nCamNum; i++)
	{
		pDev = CamArray[i];
		udev = usb_open(pDev);
		if (!udev)
			continue;

		des = &(pDev->descriptor);
		vid = des->idVendor;
		pid = des->idProduct;
		usb_get_string_simple(udev, des->iManufacturer, szMfg, 64*sizeof(char));
		printf("%3d: vid = 0x%.4x, pid = 0x%.4x, Manufacturer = %s\n", 
				i, vid, pid, szMfg);

		usb_close(udev);
	}
}

bool Burn_To_File(CBurnMgr	&burn_mgr, char szTarget[256])
{
	burn_mgr.Set_Target_File(true);
	burn_mgr.Set_Save_FileName(szTarget);
	burn_mgr.Set_Save_All(true);
	burn_mgr.Set_Save_ROM(true);
	burn_mgr.Set_Save_Des(true);
	burn_mgr.Set_Save_Param(true);
	burn_mgr.Set_Save_ISP(true);
	 
	printf("Target : Burn to FILE - %s\n\n", szTarget);
	
	printf("Start the burning process ...\n");	

	printf("Step : Init -----------------> ");
	if (!burn_mgr.Load_Source_Data())
	{
		DBG_Print("Burn_To_File : Load_Source_Data() - Fail!\n");
		printf("Fail!\n");
		return false;
	}
	printf("Pass!\n");
	
	printf("Step : Save Des -------------> ");
	if (!burn_mgr.Burn_Save_Des())
	{
		printf("Fail!\n");
		goto fail_quit;
	}
	printf("Pass!\n");
	
	printf("step : Save Param -----------> ");
	if (!burn_mgr.Burn_Save_Param())
	{
		printf("Fail!\n");
		goto fail_quit;
	}
	printf("Pass!\n");
	
	printf("Step : Save ISP -------------> ");
	if (!burn_mgr.Burn_Save_ISP())
	{
		printf("Fail!\n");
		goto fail_quit;
	}
	printf("Pass!\n");
	
	printf("Step : Save All -------------> ");
	if (!burn_mgr.Burn_Save_All())
	{
		printf("Fail!\n");
		goto fail_quit;
	}
	printf("Pass!\n");
	printf("Burn to File : Success!\n");
	return true;
	
fail_quit:
	return false;
}

bool Burn_To_Flash(CBurnMgr	&burn_mgr)
{
	char szFlashCodeVer[25] = {0};	// carol 2013/12/16 add
	char szFlashVendorVer[13] = {0};

	// shawn 2010/11/12 for fixing "No such device" issue +++++
#if defined _REINIT_	
	CCamEnum	cam_enum;
	struct usb_device* CamArray[MAX_CAM_NUM];
	int 		nCamNum;
#endif
	// shawn 2010/11/12 for fixing "No such device" issue -----
	
	burn_mgr.Set_Target_Flash(true);
	printf("Target : Burn to FLASH\n");

	// carol 2013/12/16 add +++++
	if(!burn_mgr.Get_CodeVersion(szFlashCodeVer))
	{
		DBG_Print("Get FW version Fail!\n");
		printf("Get FW version Fail!\n");
	}
	szFlashCodeVer[24] = '\0';
	printf("Current FW version: %s\n", szFlashCodeVer);
	// carol 2013/12/16 add -----
    
    // wayne 2014/06/13 add +++++
	if(!burn_mgr.Get_VendorVersion(szFlashVendorVer))
	{
		DBG_Print("Get Vendor version Fail!\n");
		printf("Get Vendor version Fail!\n");
	}
	szFlashVendorVer[13] = '\0';
	printf("Current Vendor version: %s\n", szFlashVendorVer);
    // wayne 2014/06/13 add +++++


	printf("Start the burning process ...\n");
	// ----------------- Step 0 : INIT -----------------
	printf("Step 0 : INIT ---------------> ");
	// shawn 2009/08/14 fix WP issue +++++
	if (!burn_mgr.Load_Source_Data())
	{
		DBG_Print("Burn_To_Flash : Load_Source_Data() - Fail!\n");
		printf("Fail!\n");
		goto fail_quit;
	}
	if (!burn_mgr.Burn_Init())
	{
		printf("Fail!\n");
		goto fail_quit;
	}
	// shawn 2009/08/14 fix WP issue -----
	printf("Pass!\n");
	
	// ----------------- Step 1 : ERASE -----------------	
	printf("Step 1 : ERASE --------------> ");
	if (!burn_mgr.Burn_Erase())
	{
		printf("Fail!\n");
		goto fail_quit;
	}
	printf("Pass!\n");

	// shawn 2010/11/12 for fixing "No such device" issue +++++
#if defined _REINIT_
	sleep(3);//sleep(1);
	burn_mgr.Cam_DeSelect();
	cam_enum.Set_IDCheckTable(id_table, id_num);
	if (!cam_enum.Enum_Cam(CamArray, nCamNum))
	{
		printf("enumerate webcam error!\n");
		goto fail_quit;
	}
	if (nCamNum == 0)
	{
		printf("NO webcam is found!\n");
		goto fail_quit;
	}
	
	if (!burn_mgr.Cam_Select(CamArray[0], nFileNum))
	{
		printf("Fail!\n");
		goto fail_quit;
	}
	
	if (!burn_mgr.Set_Source_File_From_INI((char *)FILE_INI))
	{
		printf("Fail!\n");
		goto fail_quit;
	}

	if (!burn_mgr.Load_Source_Data())
	{
		DBG_Print("Burn_To_Flash : Load_Source_Data() - Fail!\n");
		printf("Fail!\n");
		goto fail_quit;
	}

	if (!burn_mgr.Burn_Init())
	{
		printf("Fail!\n");
		goto fail_quit;
	}
#endif
	// shawn 2010/11/12 for fixing "No such device" issue -----

	// ----------------- Step 2 : CHECK -----------------
	printf("Step 2 : CHECK --------------> ");
	if (!burn_mgr.Burn_Check())
	{
		printf("Fail!\n");
		goto fail_quit;
	}
	printf("Pass!\n");
	// ----------------- Step 3 : PROGRAM -----------------	
	printf("Step 3 : PROGRAM ------------> ");
	if (!burn_mgr.Burn_Program())
	{
		printf("Fail!\n");
		goto fail_quit;
	}
	printf("Pass!\n");
	// ----------------- Step 4 : VERIFY -----------------	
	printf("Step 4 : VERIFY -------------> ");
	if (!burn_mgr.Burn_Verify())
	{
		printf("Fail!\n");
		goto fail_quit;
	}	
	printf("Pass!\n");

	// carol 2013/12/16 add +++++
	if(!burn_mgr.Get_CodeVersion(szFlashCodeVer))
	{
		DBG_Print("Get FW version Fail!\n");
		printf("Get FW version Fail!\n");
	}
	szFlashCodeVer[24] = '\0';
	// carol 2013/12/16 add -----
    // wayne 2014/06/13 add +++++
	if(!burn_mgr.Get_VendorVersion(szFlashVendorVer))
	{
		DBG_Print("Get Vendor version Fail!\n");
		printf("Get Vendor version Fail!\n");
	}
	szFlashVendorVer[13] = '\0';
	printf("Current Vendor version: %s\n", szFlashVendorVer);
    // wayne 2014/06/13 add +++++

	// ----------------- Step 5 : END PROCESS -----------------
	printf("Step 5 : END PROCESS --------> ");
	if (!burn_mgr.Burn_EndProc(true))
	{
		printf("Fail!\n");
		goto fail_quit;
	}	
	printf("Pass!\n");
	printf("Burn to Flash : Success!\n");

	printf("\nNew FW version: %s\n", szFlashCodeVer);
	printf("New Vendor version: %s\n", szFlashVendorVer);
	
	return true;

fail_quit:	
	printf("END PROCESS ...");
	burn_mgr.Burn_EndProc(false);
	return false;
}
int main(int argc, char *argv[])
{
	CCamEnum	cam_enum;
	struct usb_device* CamArray[MAX_CAM_NUM];
	int 		nCamNum;
	CBurnMgr	burn_mgr;
	int 		circnt = CNTNUM;

	if (argc > 1)
	{
		if ( strcmp(argv[1], "-1") == 0)
			nFileNum = 1;
		else if ( strcmp(argv[1], "-2") == 0) // carol 2013/08/29 add
			nFileNum = 2;
		else if ( strcmp(argv[1], "-3") == 0)
			nFileNum = 3;
		else if ( strcmp(argv[1], "-h") == 0)
		{
			//printf("./fw_update [-1] [-2] [-3] [-h]\n\n"); // carol 2013/10/30 temp mark for not show -3
			printf("./fw_update [-1] [-2] [-h]\n\n"); // carol 2013/10/30 add
			printf("\n-1     Burn 64k single file.\n");
			printf("\n-2     Burn 128k single file.\n"); // carol 2013/08/29 add
			//printf("\n-3     Burn three files (ROM, Parameter and Sensor Table).\n"); // carol 2013/10/30 temp mark for never try
			printf("\n-h     Help.\n");
			goto exit;
		}
		else
			;
	}
	
	if (nFileNum == 0)
	{
		printf("Input argument is wrong!\n");
		goto exit;
	}
	
	DEBUG_INIT();
	
	printf("\n%s\n%s\n%s\n\n", STR_INTRO, STR_VERSION, STR_DATE);
	LIDBG_PRINT("Prepare : enumerate webcam ...\n");
	//cam_enum.Set_IDCheckTable(id_table, id_num); // carol 2013/08/29 mark
	if (!cam_enum.Enum_Cam(CamArray, nCamNum))
	{
		LIDBG_PRINT("enumerate webcam error!\n");
		goto exit;
	}
	if (nCamNum == 0)
	{
		LIDBG_PRINT("NO webcam is found!\n");
		goto exit;
	}
	//Print_CamArray(CamArray, nCamNum);
	//printf("\n");

	LIDBG_PRINT("Prepare : select webcam #0 ... \n");
	
	if (!burn_mgr.Cam_Select(CamArray[0], nFileNum))
	{
		LIDBG_PRINT("Cam_Select Fail!\n");
		goto exit;
	}
	LIDBG_PRINT("Cam_Select OK!\n");

	printf("Prepare : read .ini file - %s ... ", FILE_INI);
	if (!burn_mgr.Set_Source_File_From_INI((char *)FILE_INI))
	{
		LIDBG_PRINT("Set_Source_File_From_INI Fail!\n");
		goto exit;
	}
	LIDBG_PRINT("Set_Source_File_From_INI OK!\n");
	printf("\n");
	
	if (nFileNum == 1)
		printf("Source : 64K from FILE                   - %s\n", burn_mgr.Get_RomFile());
	else if(nFileNum == 2) // carol 2013/08/29 add
		printf("Source : 128K from FILE                   - %s\n", burn_mgr.Get_RomFile());
	else
	{
		printf("Source : ROM from FILE                   - %s\n", burn_mgr.Get_RomFile());
		printf("Source : Parameter from FILE             - %s\n", burn_mgr.Get_ParamFile());
		printf("Source : Init Sensor Parameter from FILE - %s\n", burn_mgr.Get_ISPFile());
	}

	//Burn_To_File(burn_mgr, (char *)"target");
	
	LIDBG_PRINT("FW Burn to Flash : begin!\n");
	while((circnt--) && (!Burn_To_Flash(burn_mgr)));
	if(circnt)
		LIDBG_PRINT("FW Burn to Flash : Success! circnt = %d\n", CNTNUM-circnt);
	else
		LIDBG_PRINT("FW Burn to Flash : fail! \n");

	burn_mgr.Cam_DeSelect();

	printf("Exit program!\n");
	printf("\nPlease restart the computer to make the new FW become effective !\n");
	return 0;
	
exit:
	printf("Exit program!\n");
	return 0;
}

