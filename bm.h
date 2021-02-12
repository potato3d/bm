#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	/* ------------------------------------------------------------------------------------------------------------------------------------
	 * return codes
	 * ------------------------------------------------------------------------------------------------------------------------------------ */

	#define BM_OK    0
	#define BM_ERROR 1

	/* ------------------------------------------------------------------------------------------------------------------------------------
	 * data structures
	 * ------------------------------------------------------------------------------------------------------------------------------------ */

	#pragma pack(push)
	#pragma pack(1)

	typedef struct BmFile
	{
		void* handle;
		uint32_t version;
		uint32_t vertexTotal;
		uint32_t vertexTexturedTotal;
		uint32_t elementTotal;
		uint32_t colorTotal;
		uint32_t drawArraysTotal;
		uint32_t drawElementsTotal;
		uint32_t drawArraysTexturedTotal;
		uint32_t drawElementsTexturedTotal;
		uint32_t textureTotal;
	} BmFile;

	typedef struct BmVertex
	{
		float position[3];
		float normal[3];
	} BmVertex;

	typedef struct BmVertexTextured
	{
		float position[3];
		float normal[3];
		float texcoord[2];
	} BmVertexTextured;

	typedef unsigned int BmElement;

	typedef struct BmColor
	{
		uint8_t rgba[4];
	} BmColor;

	typedef struct BmDrawArrays
	{
		uint32_t vertexCount;
		uint32_t firstVertex;
	} BmDrawArrays;

	typedef struct BmDrawElements
	{
		uint32_t elemCount;
		uint32_t firstElem;
		uint32_t baseVertex;
	} BmDrawElements;

	typedef struct BmDrawArraysTextured
	{
		uint32_t textureID;
		BmDrawArrays drawArrays;
	} BmDrawArraysTextured;

	typedef struct BmDrawElementsTextured
	{
		uint32_t textureID;
		BmDrawElements drawElements;
	} BmDrawElementsTextured;

	typedef struct BmTextureSize
	{
		uint32_t width;
		uint32_t height;
	} BmTextureSize;

	#pragma pack(pop)

	/* ------------------------------------------------------------------------------------------------------------------------------------
	 * open/close file
	 * ------------------------------------------------------------------------------------------------------------------------------------ */

	#define BM_OPEN_READ        0
	#define BM_OPEN_READ_WRITE	1

	int bmOpen(const char* filepath, int mode, BmFile** file);
	int bmClose(BmFile* file);

	/* ------------------------------------------------------------------------------------------------------------------------------------
	 * write operations
	 * note: bm_file structure must contain the correct totals of each type, else data may be overwritten in disk
	 * ------------------------------------------------------------------------------------------------------------------------------------ */

	int bmWriteVertices(BmFile* file, uint32_t subtotal, uint32_t count, BmVertex* vertices);
	int bmWriteVerticesTextured(BmFile* file, uint32_t subtotal, uint32_t count, BmVertexTextured* vertices);
	int bmWriteElements(BmFile* file, uint32_t subtotal, uint32_t count, BmElement* elements);
	int bmWriteColors(BmFile* file, uint32_t subtotal, uint32_t count, BmColor* colors);
	int bmWriteDrawArrays(BmFile* file, uint32_t subtotal, uint32_t count, BmDrawArrays* drawArrays);
	int bmWriteDrawElements(BmFile* file, uint32_t subtotal, uint32_t count, BmDrawElements* drawElements);
	int bmWriteDrawArraysTextured(BmFile* file, uint32_t subtotal, uint32_t count, BmDrawArraysTextured* drawArraysTextured);
	int bmWriteDrawElementsTextured(BmFile* file, uint32_t subtotal, uint32_t count, BmDrawElementsTextured* drawElementsTextured);
	int bmWriteTextureSizes(BmFile* file, uint32_t subtotal, uint32_t count, BmTextureSize* textureSizes);
	int bmWriteTexels(BmFile* file, uint32_t subtotal, uint32_t count, BmColor* texels);

	/* ------------------------------------------------------------------------------------------------------------------------------------
	 * read operations
	 * note: for best performance, read all data of a given type before proceeding to the next and follow the order below
	 * ------------------------------------------------------------------------------------------------------------------------------------ */

	int bmReadVertices(BmFile* file, uint32_t first, uint32_t count, BmVertex* vertices);
	int bmReadVerticesTextured(BmFile* file, uint32_t first, uint32_t count, BmVertexTextured* vertices);
	int bmReadElements(BmFile* file, uint32_t first, uint32_t count, BmElement* elements);
	int bmReadColors(BmFile* file, uint32_t first, uint32_t count, BmColor* colors);
	int bmReadDrawArrays(BmFile* file, uint32_t first, uint32_t count, BmDrawArrays* drawArrays);
	int bmReadDrawElements(BmFile* file, uint32_t first, uint32_t count, BmDrawElements* drawElements);
	int bmReadDrawArraysTextured(BmFile* file, uint32_t first, uint32_t count, BmDrawArraysTextured* drawArraysTextured);
	int bmReadDrawElementsTextured(BmFile* file, uint32_t first, uint32_t count, BmDrawElementsTextured* drawElementsTextured);
	int bmReadTextureSizes(BmFile* file, uint32_t first, uint32_t count, BmTextureSize* textureSizes);
	int bmReadTexels(BmFile* file, uint32_t first, uint32_t count, BmColor* texels);

#ifdef __cplusplus
}
#endif
