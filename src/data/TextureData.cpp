#include "TextureData.h"
#include "stb/stb_image.h"
#include "core/Engine.h"
#include "render/Renderer.h"
#include "render/TransferList.h"

namespace
{
	static const int DESIRED_CHANNELS_COUNT = 4;
};

TextureData::TextureData(const HashString& inPath, bool inUsesAlpha /*= false*/, bool inFlipVertical /*= true*/, bool inLinear /*= true*/, bool inGenMips /*= true*/)
	: Resource(inPath)
//	, imageView(nullptr)
	, genMips(inGenMips)
	, cleanup(true)
{
	path = inPath.GetString();
	useAlpha = inUsesAlpha;
	flipVertical = inFlipVertical;
	linear = inLinear;
}

TextureData::TextureData()
	: Resource(HashString::NONE)
{

}

TextureData::~TextureData()
{
	Cleanup();
}

bool TextureData::Load()
{
	unsigned char* data;
	stbi_set_flip_vertically_on_load(flipVertical);
	data = stbi_load(path.c_str(), &width, &height, &numChannels, DESIRED_CHANNELS_COUNT);
	uint64_t dataSize = width * height * DESIRED_CHANNELS_COUNT;

	if (data == nullptr)
	{
		return false;
	}

	Device& device = Engine::GetRendererInstance()->GetDevice();
	image.resourceDescription = GetImageDesc();
	image.Create(&device);
	image.CreateStagingBuffer()->CopyTo(dataSize, reinterpret_cast<char*>(data));
	TransferList::GetInstance()->PushImage(&image);

	stbi_image_free(data);

	return true;
}

bool TextureData::Cleanup()
{
	if (!cleanup)
	{
		return false;
	}

	//if (imageView)
	//{
	//	Engine::GetRendererInstance()->GetDevice().destroyImageView(imageView);
	//	imageView = nullptr;
	//}
	if (image)
	{
		image.Destroy();
		return true;
	}
	return false;
}

void TextureData::CreateFromExternal(const ImageResource& inImage, bool inCleanup/* = false*/)
{
	image = inImage;
//	imageView = inImageView;
	cleanup = inCleanup;
}

//ImageView& TextureData::GetImageView()
//{
//	return imageView;
//}





