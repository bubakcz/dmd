/*
    Copyright (C) 2013  Martin Kuèeròák
    Contact: email:  xkucer64@stud.fit.vutbr.cz
             email2: bubak671415@seznam.cz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Description:
    (kódování: WindowsCP1250)
    Vybudování tabulky pro performance countery - AMD    
*/

#include "GPA_display_organizer.h"



void setup_hiearchy()
{
	if(!GPA_dv_hiearchy_set)
	{
		GPA_dv_hiearchy_set = true;
		GPA_dv_groups.push_back("Timing");
		GPA_dv_hiearchy["CSBusy"] = "Timing";             
		GPA_dv_hiearchy["DepthStencilTestBusy"] = "Timing";
		GPA_dv_hiearchy["DSBusy"] = "Timing";
		GPA_dv_hiearchy["GPUTime"] = "Timing";
		GPA_dv_hiearchy["GPUBusy"] = "Timing";
		GPA_dv_hiearchy["GSBusy"] = "Timing";
		GPA_dv_hiearchy["HSBusy"] = "Timing";
		GPA_dv_hiearchy["InterpBusy"] = "Timing";
		GPA_dv_hiearchy["PrimitiveAssemblyBusy"] = "Timing";
		GPA_dv_hiearchy["PSBusy"] = "Timing";
		GPA_dv_hiearchy["ShaderBusy"] = "Timing";
		GPA_dv_hiearchy["ShaderBusyCS"] = "Timing";
		GPA_dv_hiearchy["ShaderBusyDS"] = "Timing";
		GPA_dv_hiearchy["ShaderBusyGS"] = "Timing";
		GPA_dv_hiearchy["ShaderBusyHS"] = "Timing";
		GPA_dv_hiearchy["ShaderBusyPS"] = "Timing";
		GPA_dv_hiearchy["ShaderBusyVS"] = "Timing";
		GPA_dv_hiearchy["TessellatorBusy"] = "Timing";
		GPA_dv_hiearchy["TexUnitBusy"] = "Timing";
		GPA_dv_hiearchy["VSBusy"] = "Timing";

		GPA_dv_groups.push_back("VertexShader");
		GPA_dv_hiearchy["VertexMemFetched"] = "VertexShader";
		GPA_dv_hiearchy["VertexMemFetchedCost"] = "VertexShader";
		GPA_dv_hiearchy["VSALUBusy"] = "VertexShader";
		GPA_dv_hiearchy["VSALUEfficiency"] = "VertexShader";
		GPA_dv_hiearchy["VSALUInstCount"] = "VertexShader";
		GPA_dv_hiearchy["VSALUTexRatio"] = "VertexShader";
		GPA_dv_hiearchy["VSTexBusy"] = "VertexShader";
		GPA_dv_hiearchy["VSTexInstCount"] = "VertexShader";
		GPA_dv_hiearchy["VSVerticesIn"] = "VertexShader";

		GPA_dv_groups.push_back("HullShader");
		GPA_dv_hiearchy["HSALUBusy"] = "HullShader";
		GPA_dv_hiearchy["HSALUEfficiency"] = "HullShader";
		GPA_dv_hiearchy["HSALUInstCount"] = "HullShader";
		GPA_dv_hiearchy["HSALUTexRatio"] = "HullShader";
		GPA_dv_hiearchy["HSTexBusy"] = "HullShader";
		GPA_dv_hiearchy["HSTexInstCount"] = "HullShader";
		GPA_dv_hiearchy["HSPatches"] = "HullShader";

		GPA_dv_groups.push_back("DomainShader");
		GPA_dv_hiearchy["DSALUBusy"] = "DomainShader";
		GPA_dv_hiearchy["DSALUEfficiency"] = "DomainShader";
		GPA_dv_hiearchy["DSALUInstCount"] = "DomainShader";
		GPA_dv_hiearchy["DSALUTexRatio"] = "DomainShader";
		GPA_dv_hiearchy["DSTexBusy"] = "DomainShader";
		GPA_dv_hiearchy["DSTexInstCount"] = "DomainShader";
		GPA_dv_hiearchy["DSVerticesIn"] = "DomainShader";

		GPA_dv_groups.push_back("GeometryShader");
		GPA_dv_hiearchy["GSALUBusy"] = "GeometryShader";
		GPA_dv_hiearchy["GSALUEfficiency"] = "GeometryShader";
		GPA_dv_hiearchy["GSALUInstCount"] = "GeometryShader";
		GPA_dv_hiearchy["GSALUTexRatio"] = "GeometryShader";
		GPA_dv_hiearchy["GSExportPct"] = "GeometryShader";
		GPA_dv_hiearchy["GSPrimsIn"] = "GeometryShader";
		GPA_dv_hiearchy["GSTexBusy"] = "GeometryShader";
		GPA_dv_hiearchy["GSTexInstCount"] = "GeometryShader";
		GPA_dv_hiearchy["GSVerticesOut"] = "GeometryShader";

		GPA_dv_groups.push_back("PrimitiveAssembly");
		GPA_dv_hiearchy["ClippedPrims"] = "PrimitiveAssembly";
		GPA_dv_hiearchy["CulledPrims"] = "PrimitiveAssembly";
		GPA_dv_hiearchy["PAPixelsPerTriangle"] = "PrimitiveAssembly";
		GPA_dv_hiearchy["PAStalledOnRasterizer"] = "PrimitiveAssembly";
		GPA_dv_hiearchy["PrimitivesIn"] = "PrimitiveAssembly";



		GPA_dv_groups.push_back("PixelShader");
		GPA_dv_hiearchy["PSALUBusy"] = "PixelShader";
		GPA_dv_hiearchy["PSALUEfficiency"] = "PixelShader";
		GPA_dv_hiearchy["PSALUInstCount"] = "PixelShader";
		GPA_dv_hiearchy["PSALUTexRatio"] = "PixelShader";
		GPA_dv_hiearchy["PSExportStalls"] = "PixelShader";
		GPA_dv_hiearchy["PSPixelsIn"] = "PixelShader";
		GPA_dv_hiearchy["PSPixelsOut"] = "PixelShader";
		GPA_dv_hiearchy["PSTexBusy"] = "PixelShader";
		GPA_dv_hiearchy["PSTexInstCount"] = "PixelShader";

		GPA_dv_groups.push_back("TextureUnit");
		GPA_dv_hiearchy["TexAveAnisotropy"] = "TextureUnit";
		GPA_dv_hiearchy["TexCacheStalled"] = "TextureUnit";
		GPA_dv_hiearchy["TexCostOfFiltering"] = "TextureUnit";
		GPA_dv_hiearchy["TexelFetchCount"] = "TextureUnit";
		GPA_dv_hiearchy["TexMemBytesRead"] = "TextureUnit";
		GPA_dv_hiearchy["TexMissRate"] = "TextureUnit";
		GPA_dv_hiearchy["TexTriFilteringPct"] = "TextureUnit";
		GPA_dv_hiearchy["TexVolFilteringPct"] = "TextureUnit";
		GPA_dv_hiearchy["Pct64SlowTexels"] = "TextureFormat";
		GPA_dv_hiearchy["Pct128SlowTexels"] = "TextureFormat";

		GPA_dv_groups.push_back("TextureFormat");
		GPA_dv_hiearchy["PctCompressedTexels"] = "TextureFormat";
		GPA_dv_hiearchy["PctDepthTexels"] = "TextureFormat";
		GPA_dv_hiearchy["PctInterlacedTexels"] = "TextureFormat";
		GPA_dv_hiearchy["PctTex1D"] = "TextureFormat";
		GPA_dv_hiearchy["PctTex1Darray"] = "TextureFormat";
		GPA_dv_hiearchy["PctTex2D"] = "TextureFormat";
		GPA_dv_hiearchy["PctTex2Darray"] = "TextureFormat";
		GPA_dv_hiearchy["PctTex2DMSAA"] = "TextureFormat";
		GPA_dv_hiearchy["PctTex2DMSAAArray"] = "TextureFormat";
		GPA_dv_hiearchy["PctTex3D"] = "TextureFormat";
		GPA_dv_hiearchy["PctTexCube"] = "TextureFormat";
		GPA_dv_hiearchy["PctTexCubeArray"] = "TextureFormat";
		GPA_dv_hiearchy["PctUncompressedTexels"] = "TextureFormat";
		GPA_dv_hiearchy["PctVertex64SlowTexels"] = "TextureFormat";
		GPA_dv_hiearchy["PctVertex128SlowTexels"] = "TextureFormat";
		GPA_dv_hiearchy["PctVertexTexels"] = "TextureFormat";

		GPA_dv_groups.push_back("General");
		GPA_dv_hiearchy["ALUBusy"] = "General";
		GPA_dv_hiearchy["ALUFetchRatio"] = "General";
		GPA_dv_hiearchy["ALUInsts"] = "General";
		GPA_dv_hiearchy["ALUPacking"] = "General";
		GPA_dv_hiearchy["FetchInsts"] = "General";
		GPA_dv_hiearchy["GDSInsts"] = "General";
		GPA_dv_hiearchy["LDSFetchInsts"] = "General";
		GPA_dv_hiearchy["LDSWriteInsts"] = "General";
		GPA_dv_hiearchy["SALUBusy"] = "General";
		GPA_dv_hiearchy["SALUInsts"] = "General";
		GPA_dv_hiearchy["SFetchInsts"] = "General";
		GPA_dv_hiearchy["VALUBusy"] = "General";
		GPA_dv_hiearchy["VALUInsts"] = "General";
		GPA_dv_hiearchy["VALUUtilization"] = "General";
		GPA_dv_hiearchy["VFetchInsts"] = "General";
		GPA_dv_hiearchy["VWriteInsts"] = "General";
		GPA_dv_hiearchy["Wavefronts"] = "General";
		GPA_dv_hiearchy["WriteInsts"] = "General";

		GPA_dv_groups.push_back("ComputeShader");
		GPA_dv_hiearchy["CSALUBusy"] = "ComputeShader";
		GPA_dv_hiearchy["CSALUFetchRatio"] = "ComputeShader";
		GPA_dv_hiearchy["CSALUInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSALUPacking"] = "ComputeShader";
		GPA_dv_hiearchy["CSALUStalledByLDS"] = "ComputeShader";
		GPA_dv_hiearchy["CSCacheHit"] = "ComputeShader";
		GPA_dv_hiearchy["CSCompletePath"] = "ComputeShader";
		GPA_dv_hiearchy["CSFastPath"] = "ComputeShader";
		GPA_dv_hiearchy["CSFetchInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSFetchSize"] = "ComputeShader";
		GPA_dv_hiearchy["CSGDSInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSLDSBankConflict"] = "ComputeShader";
		GPA_dv_hiearchy["CSLDSFetchInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSLDSWriteInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSMemUnitBusy"] = "ComputeShader";
		GPA_dv_hiearchy["CSMemUnitStalled"] = "ComputeShader";
		GPA_dv_hiearchy["CSPathUtilization"] = "ComputeShader";
		GPA_dv_hiearchy["CSSALUBusy"] = "ComputeShader";
		GPA_dv_hiearchy["CSSALUInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSSFetchInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSFetchSize"] = "ComputeShader";
		GPA_dv_hiearchy["CSGDSInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSTexBusy"] = "ComputeShader";
		GPA_dv_hiearchy["CSThreadGroups"] = "ComputeShader";
		GPA_dv_hiearchy["CSThreads"] = "ComputeShader";
		GPA_dv_hiearchy["CSVALUBusy"] = "ComputeShader";
		GPA_dv_hiearchy["CSVALUInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSVALUUtilization"] = "ComputeShader";
		GPA_dv_hiearchy["CSVFetchInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSVWriteInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSWavefronts"] = "ComputeShader";
		GPA_dv_hiearchy["CSWriteInsts"] = "ComputeShader";
		GPA_dv_hiearchy["CSWriteSize"] = "ComputeShader";
		GPA_dv_hiearchy["CSWriteUnitStalled"] = "ComputeShader";

		GPA_dv_groups.push_back("DepthAndStencil");
		GPA_dv_hiearchy["HiZQuadsCulled"] = "DepthAndStencil";
		GPA_dv_hiearchy["HiZTilesAccepted"] = "DepthAndStencil";
		GPA_dv_hiearchy["PostZQuads"] = "DepthAndStencil";
		GPA_dv_hiearchy["PostZSamplesFailingS"] = "DepthAndStencil";
		GPA_dv_hiearchy["PostZSamplesFailingZ"] = "DepthAndStencil";
		GPA_dv_hiearchy["PostZSamplesPassing"] = "DepthAndStencil";
		GPA_dv_hiearchy["PreZQuadsCulled"] = "DepthAndStencil";
		GPA_dv_hiearchy["PreZSamplesFailingS"] = "DepthAndStencil";
		GPA_dv_hiearchy["PreZSamplesFailingZ"] = "DepthAndStencil";
		GPA_dv_hiearchy["PreZSamplesPassing"] = "DepthAndStencil";
		GPA_dv_hiearchy["PreZTilesDetailCulled"] = "DepthAndStencil";
		GPA_dv_hiearchy["ZUnitStalled"] = "DepthAndStencil";

		GPA_dv_groups.push_back("ColorBuffer");
		GPA_dv_hiearchy["CBMemRead"] = "ColorBuffer";
		GPA_dv_hiearchy["CBMemWritten"] = "ColorBuffer";
		GPA_dv_hiearchy["CBSlowPixelPct"] = "ColorBuffer";

		GPA_dv_groups.push_back("GlobalMemory");
		GPA_dv_hiearchy["CompletePath"] = "GlobalMemory";
		GPA_dv_hiearchy["FastPath"] = "GlobalMemory";
		GPA_dv_hiearchy["FetchSize"] = "GlobalMemory";
		GPA_dv_hiearchy["FetchUnitBusy"] = "GlobalMemory";
		GPA_dv_hiearchy["FetchUnitStalled"] = "GlobalMemory";
		GPA_dv_hiearchy["CacheHit"] = "GlobalMemory";
		GPA_dv_hiearchy["MemUnitBusy"] = "GlobalMemory";
		GPA_dv_hiearchy["MemUnitStalled"] = "GlobalMemory";
		GPA_dv_hiearchy["PathUtilization"] = "GlobalMemory";
		GPA_dv_hiearchy["WriteSize"] = "GlobalMemory";
		GPA_dv_hiearchy["WriteUnitStalled"] = "GlobalMemory";

		GPA_dv_groups.push_back("LocalMemory");
		GPA_dv_hiearchy["ALUStalledByLDS"] = "LocalMemory";
		GPA_dv_hiearchy["LDSBankConflict"] = "LocalMemory";		
	}
}
