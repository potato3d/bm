#include <bm/bm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------------------------------------------------------------------------
 * global constants
 * ------------------------------------------------------------------------------------------------------------------------------------ */

static const char* IDENTIFIER = "bmfile";
static const uint32_t VERSION = 1;

/* ------------------------------------------------------------------------------------------------------------------------------------
 * auxiliary functions
 * ------------------------------------------------------------------------------------------------------------------------------------ */

uint32_t getVertexOffset()
{
	return sizeof(IDENTIFIER) + sizeof(VERSION);
}

uint32_t getVertexTexturedOffset(BmFile* file)
{
	return getVertexOffset() + file->vertexTotal * sizeof(BmVertex);
}

uint32_t getElementOffset(BmFile* file)
{
	return getVertexTexturedOffset(file) + file->vertexTexturedTotal * sizeof(BmVertexTextured);
}

uint32_t getColorOffset(BmFile* file)
{
	return getElementOffset(file) + file->elementTotal * sizeof(BmElement);
}

uint32_t getDrawArraysOffset(BmFile* file)
{
	return getColorOffset(file) + file->colorTotal * sizeof(BmColor);
}

uint32_t getDrawElementsOffset(BmFile* file)
{
	return getDrawArraysOffset(file) + file->drawArraysTotal * sizeof(BmDrawArrays);
}

uint32_t getDrawArraysTexturedOffset(BmFile* file)
{
	return getDrawElementsOffset(file) + file->drawElementsTotal * sizeof(BmDrawElements);
}

uint32_t getDrawElementsTexturedOffset(BmFile* file)
{
	return getDrawArraysTexturedOffset(file) + file->drawArraysTexturedTotal * sizeof(BmDrawArraysTextured);
}

uint32_t getTextureSizeOffset(BmFile* file)
{
	return getDrawElementsTexturedOffset(file) + file->drawElementsTexturedTotal * sizeof(BmDrawElementsTextured);
}

uint32_t getTexelOffset(BmFile* file)
{
	return getTextureSizeOffset(file) + file->textureTotal * sizeof(BmTextureSize);
}

/* ------------------------------------------------------------------------------------------------------------------------------------
 * open/close file
 * ------------------------------------------------------------------------------------------------------------------------------------ */

int bmOpen(const char* filepath, int mode, BmFile** file)
{
	// check args
	if(filepath == 0 || mode < 0 || mode > 2)
	{
		return BM_ERROR;
	}

	// open file
	const char* smode = (mode == BM_OPEN_READ_WRITE) ? "w+b" : "rb";
	FILE* handle = fopen(filepath, smode);
	if(handle == 0)
	{
		return BM_ERROR;
	}

	// check identifier
	char identifier[256];
	fread(identifier, strlen(IDENTIFIER), 1, handle);
	if(strcmp(identifier, IDENTIFIER) != 0)
	{
		return BM_ERROR;
	}

	// check version
	uint32_t version;
	fread(&version, sizeof(version), 1, handle);
	if(version != VERSION)
	{
		return BM_ERROR;
	}

	// alloc structure
	BmFile* rfile = (BmFile*)malloc(sizeof(BmFile));
	rfile->handle = handle;
	rfile->version = version;

	// check if file is empty
	fseek(handle, 0, SEEK_END);
	const size_t file_size = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	// fill totals
	if(file_size == 0)
	{
		// init new file
		rfile->vertexTotal = 0;
		rfile->vertexTexturedTotal = 0;
		rfile->elementTotal = 0;
		rfile->colorTotal = 0;
		rfile->drawArraysTotal = 0;
		rfile->drawElementsTotal = 0;
		rfile->drawArraysTexturedTotal = 0;
		rfile->drawElementsTexturedTotal = 0;
		rfile->textureTotal = 0;
	}
	else
	{
		// read existing file
		size_t rcount = 0;
		rcount = fread(&rfile->vertexTotal, sizeof(rfile->vertexTotal), 1, handle);
		if(rcount != sizeof(rfile->vertexTotal))
		{
			return BM_ERROR;
		}

		rcount = fread(&rfile->vertexTexturedTotal, sizeof(rfile->vertexTexturedTotal), 1, handle);
		if(rcount != sizeof(rfile->vertexTexturedTotal))
		{
			return BM_ERROR;
		}

		rcount = fread(&rfile->elementTotal, sizeof(rfile->elementTotal), 1, handle);
		if(rcount != sizeof(rfile->elementTotal))
		{
			return BM_ERROR;
		}

		rcount = fread(&rfile->colorTotal, sizeof(rfile->colorTotal), 1, handle);
		if(rcount != sizeof(rfile->colorTotal))
		{
			return BM_ERROR;
		}

		rcount = fread(&rfile->drawArraysTotal, sizeof(rfile->drawArraysTotal), 1, handle);
		if(rcount != sizeof(rfile->drawArraysTotal))
		{
			return BM_ERROR;
		}

		rcount = fread(&rfile->drawElementsTotal, sizeof(rfile->drawElementsTotal), 1, handle);
		if(rcount != sizeof(rfile->drawElementsTotal))
		{
			return BM_ERROR;
		}

		rcount = fread(&rfile->drawArraysTexturedTotal, sizeof(rfile->drawArraysTexturedTotal), 1, handle);
		if(rcount != sizeof(rfile->drawArraysTexturedTotal))
		{
			return BM_ERROR;
		}

		rcount = fread(&rfile->drawElementsTexturedTotal, sizeof(rfile->drawElementsTexturedTotal), 1, handle);
		if(rcount != sizeof(rfile->drawElementsTexturedTotal))
		{
			return BM_ERROR;
		}

		rcount = fread(&rfile->textureTotal, sizeof(rfile->textureTotal), 1, handle);
		if(rcount != sizeof(rfile->textureTotal))
		{
			return BM_ERROR;
		}
	}

	// return result
	*file = rfile;
	return BM_OK;
}

int bmClose(BmFile* file)
{
	// check args
	if(file == 0 || file->handle == 0)
	{
		return BM_ERROR;
	}

	// close file
	if(fclose(file->handle) != 0)
	{
		return BM_ERROR;
	}

	// free memory
	free(file);

	// ok
	return BM_OK;
}

/* ------------------------------------------------------------------------------------------------------------------------------------
 * write operations
 * ------------------------------------------------------------------------------------------------------------------------------------ */

int bmWriteVertices(BmFile* file, uint32_t subtotal, uint32_t count, BmVertex* vertices)
{
	if(file == 0 || vertices == 0 || subtotal + count > file->vertexTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getVertexOffset() + subtotal * sizeof(BmVertex), SEEK_SET);
	const size_t wcount = fwrite(vertices, sizeof(BmVertex), count, file->handle);
	return (wcount == count) ? BM_OK : BM_ERROR;
}

int bmWriteVerticesTextured(BmFile* file, uint32_t subtotal, uint32_t count, BmVertexTextured* vertices)
{
	if(file == 0 || vertices == 0 || subtotal + count > file->vertexTexturedTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getVertexTexturedOffset(file) + subtotal * sizeof(BmVertexTextured), SEEK_SET);
	const size_t wcount = fwrite(vertices, sizeof(BmVertexTextured), count, file->handle);
	return (wcount == count) ? BM_OK : BM_ERROR;
}

int bmWriteElements(BmFile* file, uint32_t subtotal, uint32_t count, BmElement* elements)
{
	if(file == 0 || elements == 0 || subtotal + count > file->elementTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getElementOffset(file) + subtotal * sizeof(BmElement), SEEK_SET);
	const size_t wcount = fwrite(elements, sizeof(BmElement), count, file->handle);
	return (wcount == count) ? BM_OK : BM_ERROR;
}

int bmWriteColors(BmFile* file, uint32_t subtotal, uint32_t count, BmColor* colors)
{
	if(file == 0 || colors == 0 || subtotal + count > file->colorTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getColorOffset(file) + subtotal * sizeof(BmColor), SEEK_SET);
	const size_t wcount = fwrite(colors, sizeof(BmColor), count, file->handle);
	return (wcount == count) ? BM_OK : BM_ERROR;
}

int bmWriteDrawArrays(BmFile* file, uint32_t subtotal, uint32_t count, BmDrawArrays* drawArrays)
{
	if(file == 0 || drawArrays == 0 || subtotal + count > file->drawArraysTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getDrawArraysOffset(file) + subtotal * sizeof(BmDrawArrays), SEEK_SET);
	const size_t wcount = fwrite(drawArrays, sizeof(BmDrawArrays), count, file->handle);
	return (wcount == count) ? BM_OK : BM_ERROR;
}

int bmWriteDrawElements(BmFile* file, uint32_t subtotal, uint32_t count, BmDrawElements* drawElements)
{
	if(file == 0 || drawElements == 0 || subtotal + count > file->drawElementsTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getDrawElementsOffset(file) + subtotal * sizeof(BmDrawElements), SEEK_SET);
	const size_t wcount = fwrite(drawElements, sizeof(BmDrawElements), count, file->handle);
	return (wcount == count) ? BM_OK : BM_ERROR;
}

int bmWriteDrawArraysTextured(BmFile* file, uint32_t subtotal, uint32_t count, BmDrawArraysTextured* drawArraysTextured)
{
	if(file == 0 || drawArraysTextured == 0 || subtotal + count > file->drawArraysTexturedTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getDrawArraysTexturedOffset(file) + subtotal * sizeof(BmDrawArraysTextured), SEEK_SET);
	const size_t wcount = fwrite(drawArraysTextured, sizeof(BmDrawArraysTextured), count, file->handle);
	return (wcount == count) ? BM_OK : BM_ERROR;
}

int bmWriteDrawElementsTextured(BmFile* file, uint32_t subtotal, uint32_t count, BmDrawElementsTextured* drawElementsTextured)
{
	if(file == 0 || drawElementsTextured == 0 || subtotal + count > file->drawElementsTexturedTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getDrawElementsTexturedOffset(file) + subtotal * sizeof(BmDrawElementsTextured), SEEK_SET);
	const size_t wcount = fwrite(drawElementsTextured, sizeof(BmDrawElementsTextured), count, file->handle);
	return (wcount == count) ? BM_OK : BM_ERROR;
}

int bmWriteTextureSizes(BmFile* file, uint32_t subtotal, uint32_t count, BmTextureSize* textureSizes)
{
	if(file == 0 || textureSizes == 0 || subtotal + count > file->textureTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getTextureSizeOffset(file) + subtotal * sizeof(BmTextureSize), SEEK_SET);
	const size_t wcount = fwrite(textureSizes, sizeof(BmTextureSize), count, file->handle);
	return (wcount == count) ? BM_OK : BM_ERROR;
}

int bmWriteTexels(BmFile* file, uint32_t subtotal, uint32_t count, BmColor* texels)
{
	if(file == 0 || texels == 0 || subtotal + count > file->textureTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getTexelOffset(file) + subtotal * sizeof(BmColor), SEEK_SET);
	const size_t wcount = fwrite(texels, sizeof(BmColor), count, file->handle);
	return (wcount == count) ? BM_OK : BM_ERROR;
}

/* ------------------------------------------------------------------------------------------------------------------------------------
 * read operations
 * ------------------------------------------------------------------------------------------------------------------------------------ */

int bmReadVertices(BmFile* file, uint32_t first, uint32_t count, BmVertex* vertices)
{
	if(file == 0 || vertices == 0 || first + count > file->vertexTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getVertexOffset() + first * sizeof(BmVertex), SEEK_SET);
	const size_t rcount = fread(vertices, sizeof(BmVertex), count, file->handle);
	return (rcount == count) ? BM_OK : BM_ERROR;
}

int bmReadVerticesTextured(BmFile* file, uint32_t first, uint32_t count, BmVertexTextured* vertices)
{
	if(file == 0 || vertices == 0 || first + count > file->vertexTexturedTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getVertexTexturedOffset(file) + first * sizeof(BmVertexTextured), SEEK_SET);
	const size_t rcount = fread(vertices, sizeof(BmVertexTextured), count, file->handle);
	return (rcount == count) ? BM_OK : BM_ERROR;
}

int bmReadElements(BmFile* file, uint32_t first, uint32_t count, BmElement* elements)
{
	if(file == 0 || elements == 0 || first + count > file->elementTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getElementOffset(file) + first * sizeof(BmElement), SEEK_SET);
	const size_t rcount = fread(elements, sizeof(BmElement), count, file->handle);
	return (rcount == count) ? BM_OK : BM_ERROR;
}

int bmReadColors(BmFile* file, uint32_t first, uint32_t count, BmColor* colors)
{
	if(file == 0 || colors == 0 || first + count > file->colorTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getColorOffset(file) + first * sizeof(BmColor), SEEK_SET);
	const size_t rcount = fread(colors, sizeof(BmColor), count, file->handle);
	return (rcount == count) ? BM_OK : BM_ERROR;
}

int bmReadDrawArrays(BmFile* file, uint32_t first, uint32_t count, BmDrawArrays* drawArrays)
{
	if(file == 0 || drawArrays == 0 || first + count > file->drawArraysTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getDrawArraysOffset(file) + first * sizeof(BmDrawArrays), SEEK_SET);
	const size_t rcount = fread(drawArrays, sizeof(BmDrawArrays), count, file->handle);
	return (rcount == count) ? BM_OK : BM_ERROR;
}

int bmReadDrawElements(BmFile* file, uint32_t first, uint32_t count, BmDrawElements* drawElements)
{
	if(file == 0 || drawElements == 0 || first + count > file->drawElementsTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getDrawElementsOffset(file) + first * sizeof(BmDrawElements), SEEK_SET);
	const size_t rcount = fread(drawElements, sizeof(BmDrawElements), count, file->handle);
	return (rcount == count) ? BM_OK : BM_ERROR;
}

int bmReadDrawArraysTextured(BmFile* file, uint32_t first, uint32_t count, BmDrawArraysTextured* drawArraysTextured)
{
	if(file == 0 || drawArraysTextured == 0 || first + count > file->drawArraysTexturedTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getDrawArraysTexturedOffset(file) + first * sizeof(BmDrawArraysTextured), SEEK_SET);
	const size_t rcount = fread(drawArraysTextured, sizeof(BmDrawArraysTextured), count, file->handle);
	return (rcount == count) ? BM_OK : BM_ERROR;
}

int bmReadDrawElementsTextured(BmFile* file, uint32_t first, uint32_t count, BmDrawElementsTextured* drawElementsTextured)
{
	if(file == 0 || drawElementsTextured == 0 || first + count > file->drawElementsTexturedTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getDrawElementsTexturedOffset(file) + first * sizeof(BmDrawElementsTextured), SEEK_SET);
	const size_t rcount = fread(drawElementsTextured, sizeof(BmDrawElementsTextured), count, file->handle);
	return (rcount == count) ? BM_OK : BM_ERROR;
}

int bmReadTextureSizes(BmFile* file, uint32_t first, uint32_t count, BmTextureSize* textureSizes)
{
	if(file == 0 || textureSizes == 0 || first + count > file->textureTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getTextureSizeOffset(file) + first * sizeof(BmTextureSize), SEEK_SET);
	const size_t rcount = fread(textureSizes, sizeof(BmTextureSize), count, file->handle);
	return (rcount == count) ? BM_OK : BM_ERROR;
}

int bmReadTexels(BmFile* file, uint32_t first, uint32_t count, BmColor* texels)
{
	if(file == 0 || texels == 0 || first + count > file->textureTotal)
	{
		return BM_ERROR;
	}
	fseek(file->handle, getTexelOffset(file) + first * sizeof(BmColor), SEEK_SET);
	const size_t rcount = fread(texels, sizeof(BmColor), count, file->handle);
	return (rcount == count) ? BM_OK : BM_ERROR;
}
