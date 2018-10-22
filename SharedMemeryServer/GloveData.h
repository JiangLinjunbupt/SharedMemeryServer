#pragma once
#include <vcclr.h>  
#include<iostream>
#using "../debug/GloveTool.dll"
#using "../debug/GloveLibPC.dll"
using namespace GloveLib;
using namespace GloveTool;

class GloveData
{
public:
	float *HandinfParams;
	GloveData()
	{
		gm = gcnew GloveModel();
		gm->GetSingleton();
		gm->Mychuankou = "COM3";
		gm->Conected();
		gm->SetSocket();
		HandinfParams = new float[26];
	}
	~GloveData() {
		delete HandinfParams;
		gm->CloseSocket = true;
		gm->CloseGloveModel();
	}
	void GetGloveData()
	{
		gm->GetData();
		gm->SkeletonJsonToHandinf(gm->fram);


		HandinfParams[0] = 0;
		HandinfParams[1] = 0;
		HandinfParams[2] = 0;

		HandinfParams[3] = -gm->handinformation->global_pitch_y;
		HandinfParams[4] = gm->handinformation->global_roll_x;
		HandinfParams[5] = gm->handinformation->global_yaw_z;

		//thumb
		HandinfParams[6] = gm->handinformation->fingers[4]->Mcp_z;
		HandinfParams[7] = -gm->handinformation->fingers[4]->Mcp_x - 30;
		HandinfParams[8] = -0.6f*gm->handinformation->fingers[4]->Pip;
		HandinfParams[9] = -gm->handinformation->fingers[4]->Pip;

		//index
		HandinfParams[10] = gm->handinformation->fingers[3]->Mcp_x;
		HandinfParams[11] = gm->handinformation->fingers[3]->Mcp_z;
		HandinfParams[12] = gm->handinformation->fingers[3]->Pip;
		HandinfParams[13] = 0.8f*gm->handinformation->fingers[3]->Pip;

		//middle
		HandinfParams[14] = gm->handinformation->fingers[2]->Mcp_x;
		HandinfParams[15] = gm->handinformation->fingers[2]->Mcp_z;
		HandinfParams[16] = gm->handinformation->fingers[2]->Pip;
		HandinfParams[17] = 0.8f*gm->handinformation->fingers[2]->Pip;

		//ring
		HandinfParams[18] = gm->handinformation->fingers[1]->Mcp_x;
		HandinfParams[19] = gm->handinformation->fingers[1]->Mcp_z;
		HandinfParams[20] = gm->handinformation->fingers[1]->Pip;
		HandinfParams[21] = 0.8f*gm->handinformation->fingers[1]->Pip;

		//pinkey
		HandinfParams[22] = gm->handinformation->fingers[0]->Mcp_x;
		HandinfParams[23] = gm->handinformation->fingers[0]->Mcp_z;
		HandinfParams[24] = gm->handinformation->fingers[0]->Pip;
		HandinfParams[25] = 0.8f*gm->handinformation->fingers[0]->Pip;


		//规范参数，让手模看起来和真实更接近
		if (HandinfParams[11] < 10)
		{
			HandinfParams[11] = (HandinfParams[11] - 15.0f)<-20.0f ? -20.0f : (HandinfParams[11] - 15.0f);
		}

		if (HandinfParams[19] > -14 && HandinfParams[19] < 10.0)
		{
			HandinfParams[19] = (HandinfParams[19] + 8.0f) > 15.0f ? 15.0f : (HandinfParams[19] + 8.0f);
		}
		if (HandinfParams[23] > -19)
		{
			HandinfParams[23] = (HandinfParams[23] + 15.0f) >10.0f ? 10.0f : (HandinfParams[23] + 15.0f);
		}
		HandinfParams[15] = 0;

		if (HandinfParams[12] > 20)
		{
			HandinfParams[12] = (HandinfParams[12] + 20) > 90 ? 90 : (HandinfParams[12] + 20);
		}

		if (HandinfParams[16] > 20)
		{
			HandinfParams[16] = (HandinfParams[16] + 20) > 90 ? 90 : (HandinfParams[16] + 20);
		}

		for(int i = 0; i < 26; ++i)
		{
			if (HandinfParams[i] < -200)  HandinfParams[i] = HandinfParams[i] + 360;

			if (HandinfParams[i] > 200) HandinfParams[i] = HandinfParams[i] - 360;
		}
	}

	void SetOptimizedData(float* optimizedData)
	{
		//       0       ------>    wrist_T_x    //全局平移
		//       1       ------>    wrist_T_y    //全局平移
		//       2       ------>    wrist_T_z    //全局平移
		RestrictGlobal_xyz(-optimizedData[0], optimizedData[1], optimizedData[2]);


		//       3       ------>    wrist_R_x
		//       4       ------>    wrist_R_y
		//       5       ------>    wrist_R_z
		gm->OptimizedData->global_roll_x = optimizedData[4];
		gm->OptimizedData->global_pitch_y = -optimizedData[3];
		gm->OptimizedData->global_yaw_z = optimizedData[5];


		//       6       ------>    Thumb_Low_R_y
		//       7       ------>    Thumb_Low_R_z
		//       8       ------>    Thumb_mid_R_y    //这里注意了，是z不是y了
		//       9       ------>    Thumb_top_R_y    //这里注意了，是z不是y了
		gm->OptimizedData->fingers[0]->Mcp_x = optimizedData[6];
		gm->OptimizedData->fingers[0]->Mcp_z = optimizedData[7];
		gm->OptimizedData->fingers[0]->Pip = -optimizedData[8];
		gm->OptimizedData->fingers[0]->Dip = -optimizedData[9];


		//       10      ------>    Index_Low_R_y
		//       11      ------>    Index_Low_R_z
		//       12      ------>    Index_mid_R_y
		//       13      ------>    Index_top_R_y
		gm->OptimizedData->fingers[1]->Mcp_x = optimizedData[10];
		gm->OptimizedData->fingers[1]->Mcp_z = optimizedData[11];
		gm->OptimizedData->fingers[1]->Pip = optimizedData[12];
		gm->OptimizedData->fingers[1]->Dip = optimizedData[13];


		//       14      ------>    Middle_Low_R_y
		//       15      ------>    Middle_Low_R_z
		//       16      ------>    Middle_mid_R_y
		//       17      ------>    Middle_top_R_y
		gm->OptimizedData->fingers[2]->Mcp_x = optimizedData[14];
		gm->OptimizedData->fingers[2]->Mcp_z = optimizedData[15];
		gm->OptimizedData->fingers[2]->Pip = optimizedData[16];
		gm->OptimizedData->fingers[2]->Dip = optimizedData[17];


		//       18      ------>    Ring_Low_R_y
		//       19      ------>    Ring_Low_R_z
		//       20      ------>    Ring_mid_R_y
		//       21      ------>    Ring_top_R_y
		gm->OptimizedData->fingers[3]->Mcp_x = optimizedData[18];
		gm->OptimizedData->fingers[3]->Mcp_z = optimizedData[19];
		gm->OptimizedData->fingers[3]->Pip = optimizedData[20];
		gm->OptimizedData->fingers[3]->Dip = optimizedData[21];


		//       22      ------>    Pinkey_Low_R_y
		//       23      ------>    Pinkey_Low_R_z
		//       24      ------>    Pinkey_mid_R_y
		//       25      ------>    Pinkey_top_R_y
		gm->OptimizedData->fingers[4]->Mcp_x = optimizedData[22];
		gm->OptimizedData->fingers[4]->Mcp_z = optimizedData[23];
		gm->OptimizedData->fingers[4]->Pip = optimizedData[24];
		gm->OptimizedData->fingers[4]->Dip = optimizedData[25];

	}

	void RestrictGlobal_xyz(float X,float Y,float Z) {


		//std::cout << "X : " << X << "   Y : " << Y << "  Z  : " << Z << std::endl;
		//对位置进行限制
		float xmin = -150.0f; float xmax = 400.0f;
		float ymin = -200.0f; float ymax = 150.0f;
		float zmin = -800.0f; float zmax = -600.0f;

		if (X < xmin)
		{
			X = xmin;
		}
		if (X > xmax)
		{
			X = xmax;
		}

		if (Y < ymin)
		{
			Y = ymin;
		}
		if (Y > ymax)
		{
			Y = ymax;
		}

		if (Z < zmin)
		{
			Z = zmin;
		}
		if (Z > zmax)
		{
			Z = zmax;
		}

		gm->OptimizedData->global_x = (X - xmin) / (xmax - xmin);
		gm->OptimizedData->global_y = (Y - ymin) / (ymax - ymin);
		gm->OptimizedData->global_z = (Z - zmin) / (zmax - zmin);
	}


private:
	gcroot<GloveModel^> gm;
};
