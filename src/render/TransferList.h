#pragma once
#include <vector>
#include "resources/BufferResource.h"
#include "resources/ImageResource.h"
#include "data/MeshData.h"

class TransferList
{
public:
	static TransferList* GetInstance();

	void PushBuffer(BufferResource* inBuffer);
	void PushBuffers(MeshDataPtr inData);
	void PushImage(ImageResource* inImage);

	const std::vector<BufferResource*>& GetBuffers();
	const std::vector<ImageResource*>& GetImages();

	void ClearBuffers();
	void ClearImages();
private:
	static TransferList instance;

	std::vector<BufferResource*> buffers;
	std::vector<ImageResource*> images;

	TransferList();
	TransferList(const TransferList& inOther);
	void operator=(const TransferList& inOther);
	virtual ~TransferList();
};
