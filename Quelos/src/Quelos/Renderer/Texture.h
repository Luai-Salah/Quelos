#pragma once

#include <filesystem>

namespace Quelos
{
	enum class TextureFilterMode
	{
		Point = 0,
		Bilinear = 1,
		Tilinear = 2
	};

	enum class TextureWrapMode
	{
		Clamp = 0,
		Repeat = 1
	};

	struct TextureSpecification
	{
		TextureSpecification() = default;
		TextureSpecification(TextureFilterMode filterMode, TextureWrapMode wrapMode)
			: FilterMode(filterMode), WrapMode(wrapMode)
		{
		}

		TextureFilterMode FilterMode = TextureFilterMode::Bilinear;
		TextureWrapMode WrapMode = TextureWrapMode::Clamp;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual const TextureSpecification& GetSpecification() const = 0;
		virtual void SetSpecification(TextureSpecification spec) = 0;

		virtual const TextureFilterMode& GetFilterMode() const = 0;
		virtual void SetFilterMode(TextureFilterMode mode) = 0;

		virtual const TextureWrapMode& GetWrapMode() const = 0;
		virtual void SetWrapMode(TextureWrapMode mode) = 0;

		virtual void Apply() = 0;
		virtual bool IsApplied() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		virtual const std::filesystem::path& GetPath() const = 0;
		virtual std::string GetName() const = 0;
		virtual std::string GetNameEx() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::filesystem::path& path, const TextureSpecification& spec = TextureSpecification());
	};
}
