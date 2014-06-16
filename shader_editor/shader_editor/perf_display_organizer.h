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
*/

#pragma once
//#include <Windows.h>
#include <map>
#include <vector>
#include "shader_editor.h"

#define GPA_DV_MAJOR 1
#define GPA_DV_MINOR 2

bool perf_dv_hiearchy_set = false;
std::map<std::string, std::string> perf_dv_hiearchy;
std::map<std::string, std::string> NV_CounterTooltips;
std::vector<std::string> * perf_dv_groups;
std::vector<int> * perf_dv_group_start;

const char * get_GPA_Status_char_string(int status)
{
	switch(status)
	{
		case GPA_STATUS_OK:								return "GPA_STATUS_OK";
		case GPA_STATUS_ERROR_NULL_POINTER:				return "GPA_STATUS_ERROR_NULL_POINTER";
		case GPA_STATUS_ERROR_COUNTERS_NOT_OPEN:		return "GPA_STATUS_ERROR_COUNTERS_NOT_OPEN";
		case GPA_STATUS_ERROR_COUNTERS_ALREADY_OPEN:	return "GPA_STATUS_ERROR_COUNTERS_ALREADY_OPEN";
		case GPA_STATUS_ERROR_INDEX_OUT_OF_RANGE:		return "GPA_STATUS_ERROR_INDEX_OUT_OF_RANGE";
		case GPA_STATUS_ERROR_NOT_FOUND:				return "GPA_STATUS_ERROR_NOT_FOUND";
		case GPA_STATUS_ERROR_ALREADY_ENABLED:			return "GPA_STATUS_ERROR_ALREADY_ENABLED";
		case GPA_STATUS_ERROR_NO_COUNTERS_ENABLED:		return "GPA_STATUS_ERROR_NO_COUNTERS_ENABLED";
		case GPA_STATUS_ERROR_NOT_ENABLED:				return "GPA_STATUS_ERROR_NOT_ENABLED";
		case GPA_STATUS_ERROR_SAMPLING_NOT_STARTED:		return "GPA_STATUS_ERROR_SAMPLING_NOT_STARTED";
		case GPA_STATUS_ERROR_SAMPLING_ALREADY_STARTED: return "GPA_STATUS_ERROR_SAMPLING_ALREADY_STARTED";
		case GPA_STATUS_ERROR_SAMPLING_NOT_ENDED:		return "GPA_STATUS_ERROR_SAMPLING_NOT_ENDED";
		case GPA_STATUS_ERROR_NOT_ENOUGH_PASSES:		return "GPA_STATUS_ERROR_NOT_ENOUGH_PASSES";
		case GPA_STATUS_ERROR_PASS_NOT_ENDED:			return "GPA_STATUS_ERROR_PASS_NOT_ENDED";
		case GPA_STATUS_ERROR_PASS_NOT_STARTED:			return "GPA_STATUS_ERROR_PASS_NOT_STARTED";
		case GPA_STATUS_ERROR_PASS_ALREADY_STARTED:		return "GPA_STATUS_ERROR_PASS_ALREADY_STARTED";
		case GPA_STATUS_ERROR_SAMPLE_NOT_STARTED:		return "GPA_STATUS_ERROR_SAMPLE_NOT_STARTED";
		case GPA_STATUS_ERROR_SAMPLE_ALREADY_STARTED:	return "GPA_STATUS_ERROR_SAMPLE_ALREADY_STARTED";
		case GPA_STATUS_ERROR_SAMPLE_NOT_ENDED:			return "GPA_STATUS_ERROR_SAMPLE_NOT_ENDED";
		case GPA_STATUS_ERROR_CANNOT_CHANGE_COUNTERS_WHEN_SAMPLING: return "GPA_STATUS_ERROR_CANNOT_CHANGE_COUNTERS_WHEN_SAMPLING";
		case GPA_STATUS_ERROR_SESSION_NOT_FOUND:		return "GPA_STATUS_ERROR_SESSION_NOT_FOUND";
		case GPA_STATUS_ERROR_SAMPLE_NOT_FOUND:			return "GPA_STATUS_ERROR_SAMPLE_NOT_FOUND";
		case GPA_STATUS_ERROR_SAMPLE_NOT_FOUND_IN_ALL_PASSES:		return "GPA_STATUS_ERROR_SAMPLE_NOT_FOUND_IN_ALL_PASSES";
		case GPA_STATUS_ERROR_COUNTER_NOT_OF_SPECIFIED_TYPE:		return "GPA_STATUS_ERROR_COUNTER_NOT_OF_SPECIFIED_TYPE";
		case GPA_STATUS_ERROR_READING_COUNTER_RESULT:	return "GPA_STATUS_ERROR_READING_COUNTER_RESULT";
		case GPA_STATUS_ERROR_VARIABLE_NUMBER_OF_SAMPLES_IN_PASSES: return "GPA_STATUS_ERROR_VARIABLE_NUMBER_OF_SAMPLES_IN_PASSES";
		case GPA_STATUS_ERROR_FAILED:					return "GPA_STATUS_ERROR_FAILED";
		case GPA_STATUS_ERROR_HARDWARE_NOT_SUPPORTED:	return "GPA_STATUS_ERROR_HARDWARE_NOT_SUPPORTED";
		default: return "unrecognized status";
	}
};

QString get_GPA_Status_string(int status)
{
	return QString(get_GPA_Status_char_string(status));
}

const char * setup_hiearchy(GPU_Vendor vendor)
{
	if(!perf_dv_hiearchy_set)
	{
		perf_dv_group_start = new std::vector<int>;
		perf_dv_groups = new std::vector<std::string>;
		perf_dv_hiearchy_set = true;
		if (vendor != GPU_Vendor::NVIDIA && vendor != GPU_Vendor::ATI)
		{
			return "setup_hiearchy(GPU_Vendor vendor): nerozpoznana hodnota promenne vendor";
		}
		else if (vendor == GPU_Vendor::ATI)
		{
			perf_dv_groups->push_back("+ Timing");
			perf_dv_hiearchy["CSBusy"] = "+ Timing";             
			perf_dv_hiearchy["DepthStencilTestBusy"] = "+ Timing";
			perf_dv_hiearchy["DSBusy"] = "+ Timing";
			perf_dv_hiearchy["GPUTime"] = "+ Timing";
			perf_dv_hiearchy["GPUBusy"] = "+ Timing";
			perf_dv_hiearchy["GSBusy"] = "+ Timing";
			perf_dv_hiearchy["HSBusy"] = "+ Timing";
			perf_dv_hiearchy["InterpBusy"] = "+ Timing";
			perf_dv_hiearchy["PrimitiveAssemblyBusy"] = "+ Timing";
			perf_dv_hiearchy["PSBusy"] = "+ Timing";
			perf_dv_hiearchy["ShaderBusy"] = "+ Timing";
			perf_dv_hiearchy["ShaderBusyCS"] = "+ Timing";
			perf_dv_hiearchy["ShaderBusyDS"] = "+ Timing";
			perf_dv_hiearchy["ShaderBusyGS"] = "+ Timing";
			perf_dv_hiearchy["ShaderBusyHS"] = "+ Timing";
			perf_dv_hiearchy["ShaderBusyPS"] = "+ Timing";
			perf_dv_hiearchy["ShaderBusyVS"] = "+ Timing";
			perf_dv_hiearchy["TessellatorBusy"] = "+ Timing";
			perf_dv_hiearchy["TexUnitBusy"] = "+ Timing";
			perf_dv_hiearchy["VSBusy"] = "+ Timing";

			perf_dv_groups->push_back("+ VertexShader");
			perf_dv_hiearchy["VertexMemFetched"] = "+ VertexShader";
			perf_dv_hiearchy["VertexMemFetchedCost"] = "+ VertexShader";
			perf_dv_hiearchy["VSALUBusy"] = "+ VertexShader";
			perf_dv_hiearchy["VSALUEfficiency"] = "+ VertexShader";
			perf_dv_hiearchy["VSALUInstCount"] = "+ VertexShader";
			perf_dv_hiearchy["VSALUTexRatio"] = "+ VertexShader";
			perf_dv_hiearchy["VSTexBusy"] = "+ VertexShader";
			perf_dv_hiearchy["VSTexInstCount"] = "+ VertexShader";
			perf_dv_hiearchy["VSVerticesIn"] = "+ VertexShader";

			perf_dv_groups->push_back("+ HullShader");
			perf_dv_hiearchy["HSALUBusy"] = "+ HullShader";
			perf_dv_hiearchy["HSALUEfficiency"] = "+ HullShader";
			perf_dv_hiearchy["HSALUInstCount"] = "+ HullShader";
			perf_dv_hiearchy["HSALUTexRatio"] = "+ HullShader";
			perf_dv_hiearchy["HSTexBusy"] = "+ HullShader";
			perf_dv_hiearchy["HSTexInstCount"] = "+ HullShader";
			perf_dv_hiearchy["HSPatches"] = "+ HullShader";

			perf_dv_groups->push_back("+ GeometryShader");
			perf_dv_hiearchy["GSALUBusy"] = "+ GeometryShader";
			perf_dv_hiearchy["GSALUEfficiency"] = "+ GeometryShader";
			perf_dv_hiearchy["GSALUInstCount"] = "+ GeometryShader";
			perf_dv_hiearchy["GSALUTexRatio"] = "+ GeometryShader";
			perf_dv_hiearchy["GSExportPct"] = "+ GeometryShader";
			perf_dv_hiearchy["GSPrimsIn"] = "+ GeometryShader";
			perf_dv_hiearchy["GSTexBusy"] = "+ GeometryShader";
			perf_dv_hiearchy["GSTexInstCount"] = "+ GeometryShader";
			perf_dv_hiearchy["GSVerticesOut"] = "+ GeometryShader";

			perf_dv_groups->push_back("+ PrimitiveAssembly");
			perf_dv_hiearchy["ClippedPrims"] = "+ PrimitiveAssembly";
			perf_dv_hiearchy["CulledPrims"] = "+ PrimitiveAssembly";
			perf_dv_hiearchy["PAPixelsPerTriangle"] = "+ PrimitiveAssembly";
			perf_dv_hiearchy["PAStalledOnRasterizer"] = "+ PrimitiveAssembly";
			perf_dv_hiearchy["PrimitivesIn"] = "+ PrimitiveAssembly";

			perf_dv_groups->push_back("+ DomainShader");
			perf_dv_hiearchy["DSALUBusy"] = "+ DomainShader";
			perf_dv_hiearchy["DSALUEfficiency"] = "+ DomainShader";
			perf_dv_hiearchy["DSALUInstCount"] = "+ DomainShader";
			perf_dv_hiearchy["DSALUTexRatio"] = "+ DomainShader";
			perf_dv_hiearchy["DSTexBusy"] = "+ DomainShader";
			perf_dv_hiearchy["DSTexInstCount"] = "+ DomainShader";
			perf_dv_hiearchy["DSVerticesIn"] = "+ DomainShader";

			perf_dv_groups->push_back("+ PixelShader");
			perf_dv_hiearchy["PSALUBusy"] = "+ PixelShader";
			perf_dv_hiearchy["PSALUEfficiency"] = "+ PixelShader";
			perf_dv_hiearchy["PSALUInstCount"] = "+ PixelShader";
			perf_dv_hiearchy["PSALUTexRatio"] = "+ PixelShader";
			perf_dv_hiearchy["PSExportStalls"] = "+ PixelShader";
			perf_dv_hiearchy["PSPixelsIn"] = "+ PixelShader";
			perf_dv_hiearchy["PSPixelsOut"] = "+ PixelShader";
			perf_dv_hiearchy["PSTexBusy"] = "+ PixelShader";
			perf_dv_hiearchy["PSTexInstCount"] = "+ PixelShader";

			perf_dv_groups->push_back("+ TextureUnit");
			perf_dv_hiearchy["TexAveAnisotropy"] = "+ TextureUnit";
			perf_dv_hiearchy["TexCacheStalled"] = "+ TextureUnit";
			perf_dv_hiearchy["TexCostOfFiltering"] = "+ TextureUnit";
			perf_dv_hiearchy["TexelFetchCount"] = "+ TextureUnit";
			perf_dv_hiearchy["TexMemBytesRead"] = "+ TextureUnit";
			perf_dv_hiearchy["TexMissRate"] = "+ TextureUnit";
			perf_dv_hiearchy["TexTriFilteringPct"] = "+ TextureUnit";
			perf_dv_hiearchy["TexVolFilteringPct"] = "+ TextureUnit";
			perf_dv_hiearchy["Pct64SlowTexels"] = "TextureFormat";
			perf_dv_hiearchy["Pct128SlowTexels"] = "TextureFormat";

			perf_dv_groups->push_back("+ TextureFormat");
			perf_dv_hiearchy["PctCompressedTexels"] = "+ TextureFormat";
			perf_dv_hiearchy["PctDepthTexels"] = "+ TextureFormat";
			perf_dv_hiearchy["PctInterlacedTexels"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTex1D"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTex1Darray"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTex1DArray"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTex2D"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTex2Darray"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTex2DArray"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTex2DMSAA"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTex2DMSAAArray"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTex3D"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTexCube"] = "+ TextureFormat";
			perf_dv_hiearchy["PctTexCubeArray"] = "+ TextureFormat";
			perf_dv_hiearchy["PctUncompressedTexels"] = "TextureFormat";
			perf_dv_hiearchy["PctVertex64SlowTexels"] = "+ TextureFormat";
			perf_dv_hiearchy["PctVertex128SlowTexels"] = "+ TextureFormat";
			perf_dv_hiearchy["PctVertexTexels"] = "+ TextureFormat";

			perf_dv_groups->push_back("+ General");
			perf_dv_hiearchy["ALUBusy"] = "+ General";
			perf_dv_hiearchy["ALUFetchRatio"] = "+ General";
			perf_dv_hiearchy["ALUInsts"] = "+ General";
			perf_dv_hiearchy["ALUPacking"] = "+ General";
			perf_dv_hiearchy["FetchInsts"] = "+ General";
			perf_dv_hiearchy["GDSInsts"] = "+ General";
			perf_dv_hiearchy["LDSFetchInsts"] = "+ General";
			perf_dv_hiearchy["LDSWriteInsts"] = "+ General";
			perf_dv_hiearchy["SALUBusy"] = "+ General";
			perf_dv_hiearchy["SALUInsts"] = "+ General";
			perf_dv_hiearchy["SFetchInsts"] = "+ General";
			perf_dv_hiearchy["VALUBusy"] = "+ General";
			perf_dv_hiearchy["VALUInsts"] = "+ General";
			perf_dv_hiearchy["VALUUtilization"] = "+ General";
			perf_dv_hiearchy["VFetchInsts"] = "+ General";
			perf_dv_hiearchy["VWriteInsts"] = "+ General";
			perf_dv_hiearchy["Wavefronts"] = "+ General";
			perf_dv_hiearchy["WriteInsts"] = "+ General";

			perf_dv_groups->push_back("+ ComputeShader");
			perf_dv_hiearchy["CSALUBusy"] = "+ ComputeShader";
			perf_dv_hiearchy["CSALUFetchRatio"] = "+ ComputeShader";
			perf_dv_hiearchy["CSALUInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSALUPacking"] = "+ ComputeShader";
			perf_dv_hiearchy["CSALUStalledByLDS"] = "+ ComputeShader";
			perf_dv_hiearchy["CSCacheHit"] = "+ ComputeShader";
			perf_dv_hiearchy["CSCompletePath"] = "+ ComputeShader";
			perf_dv_hiearchy["CSFastPath"] = "+ ComputeShader";
			perf_dv_hiearchy["CSFetchInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSFetchSize"] = "+ ComputeShader";
			perf_dv_hiearchy["CSGDSInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSLDSBankConflict"] = "+ ComputeShader";
			perf_dv_hiearchy["CSLDSFetchInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSLDSWriteInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSMemUnitBusy"] = "+ ComputeShader";
			perf_dv_hiearchy["CSMemUnitStalled"] = "+ ComputeShader";
			perf_dv_hiearchy["CSPathUtilization"] = "+ ComputeShader";
			perf_dv_hiearchy["CSSALUBusy"] = "+ ComputeShader";
			perf_dv_hiearchy["CSSALUInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSSFetchInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSFetchSize"] = "+ ComputeShader";
			perf_dv_hiearchy["CSGDSInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSTexBusy"] = "+ ComputeShader";
			perf_dv_hiearchy["CSThreadGroups"] = "+ ComputeShader";
			perf_dv_hiearchy["CSThreads"] = "+ ComputeShader";
			perf_dv_hiearchy["CSVALUBusy"] = "+ ComputeShader";
			perf_dv_hiearchy["CSVALUInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSVALUUtilization"] = "+ ComputeShader";
			perf_dv_hiearchy["CSVFetchInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSVWriteInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSWavefronts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSWriteInsts"] = "+ ComputeShader";
			perf_dv_hiearchy["CSWriteSize"] = "+ ComputeShader";
			perf_dv_hiearchy["CSWriteUnitStalled"] = "+ ComputeShader";

			perf_dv_groups->push_back("+ DepthAndStencil");
			perf_dv_hiearchy["HiZQuadsCulled"] = "+ DepthAndStencil";
			perf_dv_hiearchy["HiZTilesAccepted"] = "+ DepthAndStencil";
			perf_dv_hiearchy["PostZQuads"] = "+ DepthAndStencil";
			perf_dv_hiearchy["PostZSamplesFailingS"] = "+ DepthAndStencil";
			perf_dv_hiearchy["PostZSamplesFailingZ"] = "+ DepthAndStencil";
			perf_dv_hiearchy["PostZSamplesPassing"] = "+ DepthAndStencil";
			perf_dv_hiearchy["PreZQuadsCulled"] = "+ DepthAndStencil";
			perf_dv_hiearchy["PreZSamplesFailingS"] = "+ DepthAndStencil";
			perf_dv_hiearchy["PreZSamplesFailingZ"] = "+ DepthAndStencil";
			perf_dv_hiearchy["PreZSamplesPassing"] = "+ DepthAndStencil";
			perf_dv_hiearchy["PreZTilesDetailCulled"] = "+ DepthAndStencil";
			perf_dv_hiearchy["ZUnitStalled"] = "+ DepthAndStencil";

			perf_dv_groups->push_back("+ ColorBuffer");
			perf_dv_hiearchy["CBMemRead"] = "+ ColorBuffer";
			perf_dv_hiearchy["CBMemWritten"] = "+ ColorBuffer";
			perf_dv_hiearchy["CBSlowPixelPct"] = "+ ColorBuffer";

			perf_dv_groups->push_back("+ GlobalMemory");
			perf_dv_hiearchy["CompletePath"] = "+ GlobalMemory";
			perf_dv_hiearchy["FastPath"] = "+ GlobalMemory";
			perf_dv_hiearchy["FetchSize"] = "+ GlobalMemory";
			perf_dv_hiearchy["FetchUnitBusy"] = "+ GlobalMemory";
			perf_dv_hiearchy["FetchUnitStalled"] = "+ GlobalMemory";
			perf_dv_hiearchy["CacheHit"] = "+ GlobalMemory";
			perf_dv_hiearchy["MemUnitBusy"] = "+ GlobalMemory";
			perf_dv_hiearchy["MemUnitStalled"] = "+ GlobalMemory";
			perf_dv_hiearchy["PathUtilization"] = "+ GlobalMemory";
			perf_dv_hiearchy["WriteSize"] = "+ GlobalMemory";
			perf_dv_hiearchy["WriteUnitStalled"] = "+ GlobalMemory";

			perf_dv_groups->push_back("+ LocalMemory");
			perf_dv_hiearchy["ALUStalledByLDS"] = "+ LocalMemory";
			perf_dv_hiearchy["LDSBankConflict"] = "+ LocalMemory";		
		}
		else if (vendor == GPU_Vendor::NVIDIA)
		{
			perf_dv_groups->push_back("+ Simplified Experiments");
			perf_dv_groups->push_back("+ OpenGL");
			perf_dv_groups->push_back("+ Shader");
			perf_dv_groups->push_back("+ FrameBuffer");
			perf_dv_groups->push_back("+ Texturing");
			perf_dv_groups->push_back("+ Nvidia_unsorted");

			

			perf_dv_hiearchy["gpu_idle"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["gpu_busy"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["input_assembler_busy"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["input_assembler_waits_for_fb"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["vertex_attribute_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["geom_busy"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["geom_waits_for_shader"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["geom_vertex_in_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["geom_primitive_in_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["vertex_shader_busy"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["vertex_shader_instruction_rate"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["sm_instruction_count_vertex_vsmN"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["hull_shader_instruction_rate"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["sm_instruction_count_hull_vsmN"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["domain_shader_instruction_rate"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["sm_instruction_count_domain_vsmN"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["geometry_shader_busy"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["geometry_shader_instruction_rate"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["sm_instruction_count_geometry_vsmN"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["geom_vertex_out_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["geom_primitive_out_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["stream_out_busy"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["setup_primitive_culled_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["primitive_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["setup_primitive_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["triangle_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["setup_triangle_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["setup_point_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["setup_line_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["shaded_pixel_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["rasterizer_pixels_killed_zcull_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["pixel_shader_busy"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["pixel_shader_instruction_rate"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["sm_instruction_count_pixel_vsmN"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["shader_busy"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["shader_waits_for_texture"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["shader_waits_for_geom"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["shader_waits_for_rop"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["texture_busy"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["texture_waits_for_fb"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["texture_waits_for_shader"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["texture_sample_base_level_rate"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["texture_sample_average_level"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["rop_busy"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["rop_waits_for_fb"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["rop_waits_for_shader"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["rop_pixels_killed_earlyz_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["rop_pixels_killed_latez_count"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["fb_subpN_read_sectors_fbN"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["fb_subpN_write_sectors_fbN"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["tex0_cache_sector_misses_fbN"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["tex0_cache_sector_queries_fbN"] = "+ Nvidia_unsorted";
			perf_dv_hiearchy["tex0_bank_conflicts_fbN"] = "+ Nvidia_unsorted";
// 			FPS (#)
// 				OGL FPS
// 				Frame Time (1/FPS) (#) in mSec
// 				OGL frame time
// 				Driver Sleep Time (waits for GPU) (#) in mSec
// 				OGL driver sleeping
// 				% of the Frame Time driver is waiting (%)
// 				OGL % driver waiting
// 				AGP/PCIE Memory Used in Integer MB (#)
// 				OGL AGP/PCI-E usage (MB)
// 				AGP/PCIE Memory Used in bytes (#)
// 				OGL AGP/PCI-E usage (bytes)
// 				Video Memory Used in Integer MB (#)
// 				OGL vidmem usage (MB)
// 				Video Memory Used in bytes (#)
// 				OGL vidmem usage (bytes)
// 				Total amount of video memory in bytes
// 				OGL vidmem total bytes
// 				Total amount of video memory in integer MB
// 				OGL vidmem total MB
// 				Number of batches in the frame
// 				OGL Frame Batch Count
// 				Number of vertices in the frame
// 				OGL Frame Vertex Count
// 				Number of primitives in the frame
// 				OGL Frame Primitive Count

			NV_CounterTooltips["gpu_idle"] = "The % of time the GPU is idle/busy since the last call. Having the GPU idle at all is a waste of valuable resources. You want to balance the GPU and CPU workloads so that no one processor is starved for work. Time management or using multithreading in your application can help balance CPU based tasks (world management, etc.) with the rendering pipeline.";
			NV_CounterTooltips["gpu_busy"] = "The % of time the GPU is idle/busy since the last call. Having the GPU idle at all is a waste of valuable resources. You want to balance the GPU and CPU workloads so that no one processor is starved for work. Time management or using multithreading in your application can help balance CPU based tasks (world management, etc.) with the rendering pipeline.";
			NV_CounterTooltips["input_assembler_busy"] = "The % of time the input assembler unit is busy. This is mainly impacted by both the number of vertices processed as well as the size of the attributes on those vertices. You can optimize this by reducing vertex size as much as possible and using indexed primitives to take advantage of the vertex cache.";
			NV_CounterTooltips["input_assembler_waits_for_fb"] = "+ This is the amount of time the input assembler unit was waiting for data from the frame buffer unit.";
			NV_CounterTooltips["vertex_attribute_count"] = "The number of vertex attributes that are fetched and passed to the geometry unit is returned in this counter. A large the number of attributes (or unaligned vertices) can hurt vertex cache performance and reduce the overall vertex processing capabilities of the pipeline.";
			NV_CounterTooltips["geom_busy"] = "This measures the traffic from the unit that distributes vertex and geometry work to the shader units for transforming the vertices. Draw calls with large numbers of primitives (or many draw calls with small numbers of primitives) can potentially cause this unit to become a bottleneck.";
			NV_CounterTooltips["geom_waits_for_shader"] = "The amount of time the geom unit spent waiting for the shader unit to be ready to accept work.";
			NV_CounterTooltips["geom_vertex_in_count"] = "The number of vertices input to the geom unit";
			NV_CounterTooltips["geom_primitive_in_count"] = "The number of primitives input to the geom unit";
			NV_CounterTooltips["vertex_shader_busy"] = "This is the % of time that shader unit 0 was busy scaled by the ratio of vertex shader instructions to all shader type instructions (or vertex_shader_instruction_rate). If this value is high but, for instance, pixel_shader_busy is low, it is an indication that you may be vertex/geometry bound. This can be from geometry that is too detailed or even from vertex programs that are overly complex and need to be simplified. In addition, taking advantage of the post T&L cache (by reducing vertex size and using indexed primitives) can prevent processing the same vertices multiple times.";
			NV_CounterTooltips["vertex_shader_instruction_rate"] = "The % of all shader instructions seen on the first SM unit that were executing vertex shaders";
			NV_CounterTooltips["sm_instruction_count_vertex_vsmN"] = "The number of vertex shader instructions executed on the given SM";
			NV_CounterTooltips["hull_shader_instruction_rate"] = "The % of all shader instructions seen on the first SM unit that were executing hull shaders";
			NV_CounterTooltips["sm_instruction_count_hull_vsmN"] = "The number of hull shader instructions executed on the given SM";
			NV_CounterTooltips["domain_shader_instruction_rate"] = "The % of all shader instructions seen on the first SM unit that were executing domain shaders";
			NV_CounterTooltips["sm_instruction_count_domain_vsmN"] = "The number of domain shader instructions executed on the given SM";
			NV_CounterTooltips["geometry_shader_busy"] = "This is the % of time that shader unit 0 was busy scaled by the ratio of geometry shader instructions to all shader type instructions (or geometry_shader_instruction_rate).";
			NV_CounterTooltips["geometry_shader_instruction_rate"] = "The % of all shader instructions seen on the first SM unit that were executing geometry shaders";
			NV_CounterTooltips["sm_instruction_count_geometry_vsmN"] = "The number of geometry shader instructions executed on the given SM";
			NV_CounterTooltips["geom_vertex_out_count"] = "The number of vertices coming out of the geom unit after any geometry shader expansion";
			NV_CounterTooltips["geom_primitive_out_count"] = "The number of primitives coming out of the geom unit after any geometry shader expansion";
			NV_CounterTooltips["stream_out_busy"] = "This unit manages the writing of vertices to the frame buffer when using stream out. If a significant number of vertices are written, this can become a bottleneck.";
			NV_CounterTooltips["setup_primitive_culled_count"] = "Returns the number of primitives culled in primitive setup. If you are performing viewport culling, this gives you an indication of the accuracy of the algorithm being used, and can give you an idea if you need to improve this culling. This includes primitives culled when using backface culling. Drawing a fully visible sphere on the screen should cull half of the triangles if backface culling is turned on and all the triangles are ordered consistently (CW or CCW).";
			NV_CounterTooltips["primitive_count"] = "Returns the number of primitives processed in the geometry subsystem. This experiment counts points, lines, and triangles. To count only triangles, use the triangle_count counter. Balance these counts with the number of pixels being drawn to see if you could simplify your geometry and use bump/displacement maps, for example.";
			NV_CounterTooltips["setup_primitive_count"] = "Returns the number of primitives processed in the geometry subsystem. This experiment counts points, lines, and triangles. To count only triangles, use the triangle_count counter. Balance these counts with the number of pixels being drawn to see if you could simplify your geometry and use bump/displacement maps, for example.";
			NV_CounterTooltips["triangle_count"] = "Returns the number of triangles processed in the geometry subsystem";
			NV_CounterTooltips["setup_triangle_count"] = "Returns the number of triangles processed in the geometry subsystem";
			NV_CounterTooltips["setup_point_count"] = "The number of points seen by the primitive setup unit (just before rasterization)";
			NV_CounterTooltips["setup_line_count"] = "The number of lines seen by the primitive setup unit (just before rasterization)";
			NV_CounterTooltips["shaded_pixel_count"] = "Counts the number of pixels generated by the rasterizer and sent to the pixel shader units.";
			NV_CounterTooltips["rasterizer_pixels_killed_zcull_count"] = "The number of pixels killed by the zcull unit in the rasterizer";
			NV_CounterTooltips["pixel_shader_busy"] = "This is the % of time that shader unit 0 was busy scaled by the ratio of pixel shader instructions to all shader type instructions (or pixel_shader_instruction_rate). This can give you an indication of if you are pixel bound, which can happen in high resolution settings or when pixel programs are very complex.";
			NV_CounterTooltips["pixel_shader_instruction_rate"] = "The % of all shader instructions seen on the first SM unit that were executing pixel shaders";
			NV_CounterTooltips["sm_instruction_count_pixel_vsmN"] = "The number of pixel shader instructions executed on the given SM";
			NV_CounterTooltips["shader_busy"] = "This measures the how active the unified shader unit is running any type of shader. If you couple this information with the various shader_instruction_rate values you can get an idea for the workload the shader unit has and which shader types to tune if the shader unit becomes a bottleneck.";
			NV_CounterTooltips["shader_waits_for_texture"] = "This is the amount of time that the pixel shader unit was stalled waiting for a texture fetch. Texture stalls usually happen if textures don’t have mipmaps, if a high level of anisotropic filtering is used, or if there is poor coherency in accessing textures.";
			NV_CounterTooltips["shader_waits_for_geom"] = "This is the amount of time the shader unit spent waiting for the geom unit to send work.";
			NV_CounterTooltips["shader_waits_for_rop"] = "This is the % of time that the pixel shader is stalled by the raster operations unit (ROP), waiting to blend a pixel and write it to the frame buffer. If the application is performing a lot of alpha blending, or even if the application has a lot of overdraw (the same pixel being written multiple times, unblended) this can be a performance bottleneck.";
			NV_CounterTooltips["texture_busy"] = "This is a measurement of how busy the texture unit is. This covers both time spent sampling values from the frame buffer (through the texture cache) and the time spent computing any filtering that is enabled (like bilinear or anisotropic). Reducing both the number of taps and the filtering level will help to reduce any bottleneck in the texture unit.";
			NV_CounterTooltips["texture_waits_for_fb"] = "This is the amount of time the texture unit spent waiting on samples to return from the frame buffer unit. It is a potential indication of poor texture cache utilization.";
			NV_CounterTooltips["texture_waits_for_shader"] = "This is the amount of time the texture unit spent waiting to send results to the shader unit. If the queue between those units gets too full (because the shader unit isn’t ready to receive those values), this can become a bottleneck.";
			NV_CounterTooltips["texture_sample_base_level_rate"] = "The percentage of texture samples which read from the base texture level. This can be useful to determine if your base texture level is too large and can be reduced to the next mipmap level.";
			NV_CounterTooltips["texture_sample_average_level"] = "The average LOD sourced across all texture reads.";
			NV_CounterTooltips["rop_busy"] = "% of time that the ROP unit is actively doing work. This can be high if alpha blending is turned on, of overdraw is high, etc.";
			NV_CounterTooltips["rop_waits_for_fb"] = "The amount of time the blending unit spent waiting for data from the frame buffer unit. If blending is enabled and there is a lot of traffic here (since this is a read/modify/write operation) this can become a bottleneck.";
			NV_CounterTooltips["rop_waits_for_shader"] = "This is a measurement of how often the blending unit was waiting on new work (fragments to be placed into the render target). If the pixel shaders are particularly expensive, the ROP unit could be starved waiting for results.";
			NV_CounterTooltips["rop_pixels_killed_earlyz_count"] = "This returns the number of pixels that were killed in the earlyZ hardware. This signal will give you an idea of, for instance, a Z only pass was successful in setting up the depth buffer.";
			NV_CounterTooltips["rop_pixels_killed_latez_count"] = "This returns the number of pixels that were killed after the pixel shader ran. This can happen if the early Z is unable cull the pixel because of an API setup issue like changing the Z direction or modifying Z in the pixel shader.";
			NV_CounterTooltips["fb_subpN_read_sectors_fbN"] = "The number of sectors read from the given sub partition on the specified frame buffer unit";
			NV_CounterTooltips["fb_subpN_write_sectors_fbN"] = "The number of sectors written to the given sub partition on the specified frame buffer unit";
			NV_CounterTooltips["tex0_cache_sector_misses_fbN"] = "The number of texture cache misses from unit X for the specified GPC and TPC";
			NV_CounterTooltips["tex0_cache_sector_queries_fbN"] = "The number of texture cache queries from unit X for the specified GPC and TPC";
			NV_CounterTooltips["tex0_bank_conflicts_fbN"] = "The number of texture cache conflicts from unit X for the specified GPC and TPC";

			NV_CounterTooltips["IA Bottleneck"] = "IA is the unit on Fermi and Kepler GPUs that handles vertex attribute assembly.";
			NV_CounterTooltips["IA SOL"] = "IA is the unit on Fermi and Kepler GPUs that handles vertex attribute assembly.";
			NV_CounterTooltips["Primitive Setup Bottleneck"] = "Primitive setup happens right before rasterization and handles jobs like edge equation.";
			NV_CounterTooltips["Primitive Setup SOL"] = "Primitive setup happens right before rasterization and handles jobs like edge equation.";
			NV_CounterTooltips["Rasterization Bottleneck"] = "Rasterization is when the primitives are split up into individual fragments to be shaded.";
			NV_CounterTooltips["Rasterization SOL"] = "Rasterization is when the primitives are split up into individual fragments to be shaded.";
			NV_CounterTooltips["FB Bottleneck"] = "The FB or frame buffer unit handles all request for reading memory that missed any possible L1/L2 caches.";
			NV_CounterTooltips["FB SOL"] = "The FB or frame buffer unit handles all request for reading memory that missed any possible L1/L2 caches.";
			NV_CounterTooltips["ROP Bottleneck"] = "ROP is the blending unit and handles both color blending and Z/stencil buffer handling.";
			NV_CounterTooltips["ROP SOL"] = "ROP is the blending unit and handles both color blending and Z/stencil buffer handling.";
			NV_CounterTooltips["ZCull Bottleneck"] = "ZCull happens before the fragment shader is run and is able to discard fragments because they won’t pass the z-test.";
			NV_CounterTooltips["ZCull SOL"] = "ZCull happens before the fragment shader is run and is able to discard fragments because they won’t pass the z-test.";
			NV_CounterTooltips["SHD Bottleneck"] = "SHD is the unified shader unit and handles processing of all shader types on various inputs.";
			NV_CounterTooltips["SHD SOL"] = "SHD is the unified shader unit and handles processing of all shader types on various inputs.";
			NV_CounterTooltips["TEX Bottleneck"] = "The TEX unit is responsible for reading samples from the frame buffer and calculating the texel value based on the current filtering mode.";
			NV_CounterTooltips["TEX SOL"] = "The TEX unit is responsible for reading samples from the frame buffer and calculating the texel value based on the current filtering mode.";
		}
	}
	return NULL;
}

char * getCustomGroup(const char * counterName)
{
	std::string counterNameString = counterName;

	int findPos[20];
	findPos[0] = counterNameString.find("OGL");

	if (findPos[0]!=std::string::npos)
	{
		return "+ OpenGL";
	}
	findPos[1] = counterNameString.find("IDX ");
	findPos[2] = counterNameString.find("IA ");
	findPos[3] = counterNameString.find("GEOM ");
	findPos[4] = counterNameString.find("Primitive Setup ");
	findPos[5] = counterNameString.find("SHD ");
	findPos[6] = counterNameString.find("ZCULL ");
	findPos[11] = counterNameString.find("ZCull ");
	findPos[7] = counterNameString.find("TEX ");
	findPos[8] = counterNameString.find("ROP ");
	findPos[9] = counterNameString.find("Rasterization ");
	findPos[10] = counterNameString.find("FB ");
	findPos[12] = counterNameString.find("GPU Bottleneck");
	for (int i=1; i<=12;i++)
	{
		if (findPos[i]!=std::string::npos)
		{
			findPos[0] = findPos[i];
			break;
		}
	}
	if (findPos[0]!=std::string::npos)
	{
		return "+ Simplified Experiments";
	}

	findPos[0] = counterNameString.find("fb_");
	if (findPos[0]!=std::string::npos)
	{
		return "+ FrameBuffer";
	}
	//"+ Texturing"
	findPos[0] = counterNameString.find("instruction");
	if (findPos[0]!=std::string::npos)
	{
		return "+ Shader";
	}
	findPos[0] = counterNameString.substr(0,3).find("tex");
	if (findPos[0]!=std::string::npos)
	{
		return "+ Texturing";
	}
	//"+ FrameBuffer"

	return "+ Nvidia_unsorted";
}

char * getCustomGroup(char * counterName)
{
	return getCustomGroup((const char * )counterName);
}




