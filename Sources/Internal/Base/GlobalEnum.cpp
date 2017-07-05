#include "Base/GlobalEnum.h"
#include "Engine/EngineTypes.h"
#include "Render/Texture.h"
#include "Render/Highlevel/Light.h"
#include "Scene3D/Entity.h"
#include "Platform/DeviceInfo.h"
#include "DLC/DLC.h"
#include "UI/UIControlBackground.h"
#include "UI/UIStaticText.h"
#include "Render/2D/TextBlock.h"
#include "UI/UIList.h"
#include "UI/UITextField.h"
#include "UI/Components/UIComponent.h"
#include "UI/Layouts/UISizePolicyComponent.h"
#include "UI/Layouts/UILinearLayoutComponent.h"
#include "UI/Layouts/UIFlowLayoutComponent.h"
#include "UI/Focus/UIFocusComponent.h"
#include "UI/UIEvent.h"
#include "Input/KeyboardShortcut.h"
#include "Logger/Logger.h"
#include "UI/UIWebView.h"
#include "Render/RHI/rhi_Type.h"
#include "Render/Highlevel/BillboardRenderObject.h"
#include "Render/Highlevel/GeoDecalManager.h"
#include "Utils/BiDiHelper.h"

using namespace DAVA;

ENUM_DECLARE(eGPUFamily)
{
    ENUM_ADD_DESCR(GPU_POWERVR_IOS, "PowerVR_iOS");
    ENUM_ADD_DESCR(GPU_POWERVR_ANDROID, "PowerVR_Android");
    ENUM_ADD_DESCR(GPU_TEGRA, "tegra");
    ENUM_ADD_DESCR(GPU_MALI, "mali");
    ENUM_ADD_DESCR(GPU_ADRENO, "adreno");
    ENUM_ADD_DESCR(GPU_DX11, "dx11");
    ENUM_ADD_DESCR(GPU_ORIGIN, "origin");
    ENUM_ADD_DESCR(GPU_FAMILY_COUNT, "all GPU");
}

ENUM_DECLARE(Logger::eLogLevel)
{
    ENUM_ADD_DESCR(Logger::LEVEL_FRAMEWORK, "Framework");
    ENUM_ADD_DESCR(Logger::LEVEL_DEBUG, "Debug");
    ENUM_ADD_DESCR(Logger::LEVEL_INFO, "Info");
    ENUM_ADD_DESCR(Logger::LEVEL_WARNING, "Warning");
    ENUM_ADD_DESCR(Logger::LEVEL_ERROR, "Error");
}

ENUM_DECLARE(PixelFormat)
{
    ENUM_ADD_DESCR(FORMAT_RGBA8888, "RGBA8888");
    ENUM_ADD_DESCR(FORMAT_RGBA5551, "RGBA5551");
    ENUM_ADD_DESCR(FORMAT_RGBA4444, "RGBA4444");
    ENUM_ADD_DESCR(FORMAT_RGB888, "RGB888");
    ENUM_ADD_DESCR(FORMAT_RGB565, "RGB565");
    ENUM_ADD_DESCR(FORMAT_A8, "A8");
    ENUM_ADD_DESCR(FORMAT_A16, "A16");
    ENUM_ADD_DESCR(FORMAT_PVR4, "PVR4");
    ENUM_ADD_DESCR(FORMAT_PVR2, "PVR2");
    ENUM_ADD_DESCR(FORMAT_RGBA16161616, "RGBA16161616");
    ENUM_ADD_DESCR(FORMAT_RGBA32323232, "RGBA32323232");
    ENUM_ADD_DESCR(FORMAT_R16F, "R16F");
    ENUM_ADD_DESCR(FORMAT_RG16F, "RG16F");
    ENUM_ADD_DESCR(FORMAT_RGB16F, "RGB16F");
    ENUM_ADD_DESCR(FORMAT_RGBA16F, "RGBA16F");
    ENUM_ADD_DESCR(FORMAT_R32F, "R32F");
    ENUM_ADD_DESCR(FORMAT_RG32F, "RG32F");
    ENUM_ADD_DESCR(FORMAT_RGB32F, "RGB32F");
    ENUM_ADD_DESCR(FORMAT_RGBA32F, "RGBA32F");
    ENUM_ADD_DESCR(FORMAT_DXT1, "DXT1");
    ENUM_ADD_DESCR(FORMAT_DXT1A, "DXT1A");
    ENUM_ADD_DESCR(FORMAT_DXT3, "DXT3");
    ENUM_ADD_DESCR(FORMAT_DXT5, "DXT5");
    ENUM_ADD_DESCR(FORMAT_DXT5NM, "DXT5NM");
    ENUM_ADD_DESCR(FORMAT_ETC1, "ETC1");
    ENUM_ADD_DESCR(FORMAT_ATC_RGB, "ATC_RGB");
    ENUM_ADD_DESCR(FORMAT_ATC_RGBA_EXPLICIT_ALPHA, "ATC_RGBA_EXPLICIT_ALPHA");
    ENUM_ADD_DESCR(FORMAT_ATC_RGBA_INTERPOLATED_ALPHA, "ATC_RGBA_INTERPOLATED_ALPHA");

    ENUM_ADD_DESCR(FORMAT_PVR2_2, "PVR2_2");
    ENUM_ADD_DESCR(FORMAT_PVR4_2, "PVR4_2");
    ENUM_ADD_DESCR(FORMAT_EAC_R11_UNSIGNED, "EAC_R11_UNSIGNED");
    ENUM_ADD_DESCR(FORMAT_EAC_R11_SIGNED, "EAC_R11_SIGNED");
    ENUM_ADD_DESCR(FORMAT_EAC_RG11_UNSIGNED, "EAC_RG11_UNSIGNED");
    ENUM_ADD_DESCR(FORMAT_EAC_RG11_SIGNED, "EAC_RG11_SIGNED");
    ENUM_ADD_DESCR(FORMAT_ETC2_RGB, "ETC2_RGB");
    ENUM_ADD_DESCR(FORMAT_ETC2_RGBA, "ETC2_RGBA");
    ENUM_ADD_DESCR(FORMAT_ETC2_RGB_A1, "ETC2_RGB_A1");
}

ENUM_DECLARE(ImageFormat)
{
    ENUM_ADD_DESCR(ImageFormat::IMAGE_FORMAT_PNG, "PNG");
    ENUM_ADD_DESCR(ImageFormat::IMAGE_FORMAT_DDS, "DDS");
    ENUM_ADD_DESCR(ImageFormat::IMAGE_FORMAT_PVR, "PVR");
    ENUM_ADD_DESCR(ImageFormat::IMAGE_FORMAT_JPEG, "JPEG");
    ENUM_ADD_DESCR(ImageFormat::IMAGE_FORMAT_TGA, "TGA");
    ENUM_ADD_DESCR(ImageFormat::IMAGE_FORMAT_WEBP, "WEBP");
    ENUM_ADD_DESCR(ImageFormat::IMAGE_FORMAT_PSD, "PSD");
    ENUM_ADD_DESCR(ImageFormat::IMAGE_FORMAT_HDR, "HDR");
    ENUM_ADD_DESCR(ImageFormat::IMAGE_FORMAT_UNKNOWN, "Unknown");
}

ENUM_DECLARE(Light::eType)
{
    ENUM_ADD_DESCR(Light::TYPE_DIRECTIONAL, "Directional");
    ENUM_ADD_DESCR(Light::TYPE_SPOT, "Spot");
    ENUM_ADD_DESCR(Light::TYPE_POINT, "Point");
    ENUM_ADD_DESCR(Light::TYPE_SKY, "Sky");
    ENUM_ADD_DESCR(Light::TYPE_AMBIENT, "Ambient");
}

ENUM_DECLARE(Entity::EntityFlags)
{
    ENUM_ADD_DESCR(Entity::NODE_WORLD_MATRIX_ACTUAL, "NODE_WORLD_MATRIX_ACTUAL");
    ENUM_ADD_DESCR(Entity::NODE_VISIBLE, "NODE_VISIBLE");
    ENUM_ADD_DESCR(Entity::NODE_UPDATABLE, "NODE_UPDATABLE");
    ENUM_ADD_DESCR(Entity::NODE_IS_LOD_PART, "NODE_IS_LOD_PART");
    ENUM_ADD_DESCR(Entity::NODE_LOCAL_MATRIX_IDENTITY, "NODE_LOCAL_MATRIX_IDENTITY");
    ENUM_ADD_DESCR(Entity::BOUNDING_VOLUME_AABB, "BOUNDING_VOLUME_AABB");
    ENUM_ADD_DESCR(Entity::BOUNDING_VOLUME_OOB, "BOUNDING_VOLUME_OOB");
    ENUM_ADD_DESCR(Entity::BOUNDING_VOLUME_SPHERE, "BOUNDING_VOLUME_SPHERE");
    ENUM_ADD_DESCR(Entity::NODE_CLIPPED_PREV_FRAME, "NODE_CLIPPED_PREV_FRAME");
    ENUM_ADD_DESCR(Entity::NODE_CLIPPED_THIS_FRAME, "NODE_CLIPPED_THIS_FRAME");
    ENUM_ADD_DESCR(Entity::NODE_INVALID, "NODE_INVALID");
    ENUM_ADD_DESCR(Entity::TRANSFORM_NEED_UPDATE, "TRANSFORM_NEED_UPDATE");
    ENUM_ADD_DESCR(Entity::TRANSFORM_DIRTY, "TRANSFORM_DIRTY");
    ENUM_ADD_DESCR(Entity::NODE_DELETED, "NODE_DELETED");
    ENUM_ADD_DESCR(Entity::SCENE_LIGHTS_MODIFIED, "SCENE_LIGHTS_MODIFIED");
}

ENUM_DECLARE(DeviceInfo::ePlatform)
{
    ENUM_ADD_DESCR(DeviceInfo::PLATFORM_IOS, "iOS");
    ENUM_ADD_DESCR(DeviceInfo::PLATFORM_IOS_SIMULATOR, "iOS Simulator");
    ENUM_ADD_DESCR(DeviceInfo::PLATFORM_MACOS, "MacOS");
    ENUM_ADD_DESCR(DeviceInfo::PLATFORM_ANDROID, "Android");
    ENUM_ADD_DESCR(DeviceInfo::PLATFORM_WIN32, "Win32");
    ENUM_ADD_DESCR(DeviceInfo::PLATFORM_DESKTOP_WIN_UAP, "Windows Desktop UAP");
    ENUM_ADD_DESCR(DeviceInfo::PLATFORM_PHONE_WIN_UAP, "Windows Phone UAP");
    ENUM_ADD_DESCR(DeviceInfo::PLATFORM_LINUX, "Linux");
    ENUM_ADD_DESCR(DeviceInfo::PLATFORM_UNKNOWN_VALUE, "Unknown");
}

ENUM_DECLARE(DLC::DLCError)
{
    ENUM_ADD(DLC::DE_NO_ERROR);
    ENUM_ADD(DLC::DE_WAS_CANCELED);
    ENUM_ADD(DLC::DE_INIT_ERROR);
    ENUM_ADD(DLC::DE_CHECK_ERROR);
    ENUM_ADD(DLC::DE_READ_ERROR);
    ENUM_ADD(DLC::DE_WRITE_ERROR);
    ENUM_ADD(DLC::DE_CONNECT_ERROR);
    ENUM_ADD(DLC::DE_DOWNLOAD_ERROR);
    ENUM_ADD(DLC::DE_PATCH_ERROR_LITE);
    ENUM_ADD(DLC::DE_PATCH_ERROR_FULL);
}

ENUM_DECLARE(Interpolation::FuncType)
{
    ENUM_ADD_DESCR(Interpolation::LINEAR, "LINEAR");
    ENUM_ADD_DESCR(Interpolation::EASE_IN, "EASE_IN");
    ENUM_ADD_DESCR(Interpolation::EASE_OUT, "EASE_OUT");
    ENUM_ADD_DESCR(Interpolation::EASE_IN_EASY_OUT, "EASE_IN_EASY_OUT");
    ENUM_ADD_DESCR(Interpolation::SINE_IN, "SINE_IN");
    ENUM_ADD_DESCR(Interpolation::SINE_OUT, "SINE_OUT");
    ENUM_ADD_DESCR(Interpolation::SINE_IN_SINE_OUT, "SINE_IN_SINE_OUT");
    ENUM_ADD_DESCR(Interpolation::ELASTIC_IN, "ELASTIC_IN");
    ENUM_ADD_DESCR(Interpolation::ELASTIC_OUT, "ELASTIC_OUT");
    ENUM_ADD_DESCR(Interpolation::ELASTIC_IN_ELASTIC_OUT, "ELASTIC_IN_ELASTIC_OUT");
    ENUM_ADD_DESCR(Interpolation::BOUNCE_IN, "BOUNCE_IN");
    ENUM_ADD_DESCR(Interpolation::BOUNCE_OUT, "BOUNCE_OUT");
    ENUM_ADD_DESCR(Interpolation::BOUNCE_IN_BOUNCE_OUT, "BOUNCE_IN_BOUNCE_OUT");
}

ENUM_DECLARE(UIControlBackground::eDrawType)
{
    ENUM_ADD_DESCR(UIControlBackground::DRAW_ALIGNED, "DRAW_ALIGNED");
    ENUM_ADD_DESCR(UIControlBackground::DRAW_SCALE_TO_RECT, "DRAW_SCALE_TO_RECT");
    ENUM_ADD_DESCR(UIControlBackground::DRAW_SCALE_PROPORTIONAL, "DRAW_SCALE_PROPORTIONAL");
    ENUM_ADD_DESCR(UIControlBackground::DRAW_SCALE_PROPORTIONAL_ONE, "DRAW_SCALE_PROPORTIONAL_ONE");
    ENUM_ADD_DESCR(UIControlBackground::DRAW_FILL, "DRAW_FILL");
    ENUM_ADD_DESCR(UIControlBackground::DRAW_STRETCH_HORIZONTAL, "DRAW_STRETCH_HORIZONTAL");
    ENUM_ADD_DESCR(UIControlBackground::DRAW_STRETCH_VERTICAL, "DRAW_STRETCH_VERTICAL");
    ENUM_ADD_DESCR(UIControlBackground::DRAW_STRETCH_BOTH, "DRAW_STRETCH_BOTH");
    ENUM_ADD_DESCR(UIControlBackground::DRAW_TILED, "DRAW_TILED");
    ENUM_ADD_DESCR(UIControlBackground::DRAW_TILED_MULTILAYER, "DRAW_TILED_MULTILAYER");
    ENUM_ADD_DESCR(UIControlBackground::DRAW_BATCH, "DRAW_BATCH");
}

ENUM_DECLARE(eAlign)
{
    ENUM_ADD_DESCR(ALIGN_LEFT, "LEFT");
    ENUM_ADD_DESCR(ALIGN_HCENTER, "HCENTER");
    ENUM_ADD_DESCR(ALIGN_RIGHT, "RIGHT");
    ENUM_ADD_DESCR(ALIGN_TOP, "TOP");
    ENUM_ADD_DESCR(ALIGN_VCENTER, "VCENTER");
    ENUM_ADD_DESCR(ALIGN_BOTTOM, "BOTTOM");
    ENUM_ADD_DESCR(ALIGN_HJUSTIFY, "HJUSTIFY");
}

ENUM_DECLARE(UIControlBackground::eColorInheritType)
{
    ENUM_ADD_DESCR(UIControlBackground::COLOR_MULTIPLY_ON_PARENT, "COLOR_MULTIPLY_ON_PARENT");
    ENUM_ADD_DESCR(UIControlBackground::COLOR_ADD_TO_PARENT, "COLOR_ADD_TO_PARENT");
    ENUM_ADD_DESCR(UIControlBackground::COLOR_REPLACE_TO_PARENT, "COLOR_REPLACE_TO_PARENT");
    ENUM_ADD_DESCR(UIControlBackground::COLOR_IGNORE_PARENT, "COLOR_IGNORE_PARENT");
    ENUM_ADD_DESCR(UIControlBackground::COLOR_MULTIPLY_ALPHA_ONLY, "COLOR_MULTIPLY_ALPHA_ONLY");
    ENUM_ADD_DESCR(UIControlBackground::COLOR_REPLACE_ALPHA_ONLY, "COLOR_REPLACE_ALPHA_ONLY");
}

ENUM_DECLARE(UIControlBackground::ePerPixelAccuracyType)
{
    ENUM_ADD_DESCR(UIControlBackground::PER_PIXEL_ACCURACY_DISABLED, "PER_PIXEL_ACCURACY_DISABLED");
    ENUM_ADD_DESCR(UIControlBackground::PER_PIXEL_ACCURACY_ENABLED, "PER_PIXEL_ACCURACY_ENABLED");
    ENUM_ADD_DESCR(UIControlBackground::PER_PIXEL_ACCURACY_FORCED, "PER_PIXEL_ACCURACY_FORCED");
};

ENUM_DECLARE(eSpriteModification)
{
    ENUM_ADD_DESCR(eSpriteModification::ESM_HFLIP, "FLIP_HORIZONTAL");
    ENUM_ADD_DESCR(eSpriteModification::ESM_VFLIP, "FLIP_VERTICAL");
};

ENUM_DECLARE(UIStaticText::eMultiline)
{
    ENUM_ADD_DESCR(UIStaticText::MULTILINE_DISABLED, "MULTILINE_DISABLED");
    ENUM_ADD_DESCR(UIStaticText::MULTILINE_ENABLED, "MULTILINE_ENABLED");
    ENUM_ADD_DESCR(UIStaticText::MULTILINE_ENABLED_BY_SYMBOL, "MULTILINE_ENABLED_BY_SYMBOL");
};

ENUM_DECLARE(TextBlock::eFitType)
{
    ENUM_ADD_DESCR(TextBlock::FITTING_ENLARGE, "ENLARGE");
    ENUM_ADD_DESCR(TextBlock::FITTING_REDUCE, "REDUCE");
    ENUM_ADD_DESCR(TextBlock::FITTING_POINTS, "POINTS");
};

ENUM_DECLARE(TextBlock::eUseRtlAlign)
{
    ENUM_ADD_DESCR(TextBlock::RTL_DONT_USE, "DONT_USE");
    ENUM_ADD_DESCR(TextBlock::RTL_USE_BY_CONTENT, "USE_BY_CONTENT");
    ENUM_ADD_DESCR(TextBlock::RTL_USE_BY_SYSTEM, "USE_BY_SYSTEM");
};

ENUM_DECLARE(BiDiHelper::Direction)
{
    ENUM_ADD_DESCR(BiDiHelper::RTL, "RTL");
    ENUM_ADD_DESCR(BiDiHelper::LTR, "LTR");
    ENUM_ADD_DESCR(BiDiHelper::NEUTRAL, "NEUTRAL");
    ENUM_ADD_DESCR(BiDiHelper::MIXED, "MIXED");
};

ENUM_DECLARE(UIList::eListOrientation)
{
    ENUM_ADD_DESCR(UIList::ORIENTATION_VERTICAL, "ORIENTATION_VERTICAL");
    ENUM_ADD_DESCR(UIList::ORIENTATION_HORIZONTAL, "ORIENTATION_HORIZONTAL");
};

ENUM_DECLARE(UIScrollBar::eScrollOrientation)
{
    ENUM_ADD_DESCR(UIScrollBar::ORIENTATION_VERTICAL, "ORIENTATION_VERTICAL");
    ENUM_ADD_DESCR(UIScrollBar::ORIENTATION_HORIZONTAL, "ORIENTATION_HORIZONTAL");
};

ENUM_DECLARE(UITextField::eAutoCapitalizationType)
{
    ENUM_ADD_DESCR(UITextField::AUTO_CAPITALIZATION_TYPE_NONE, "AUTO_CAPITALIZATION_TYPE_NONE");
    ENUM_ADD_DESCR(UITextField::AUTO_CAPITALIZATION_TYPE_WORDS, "AUTO_CAPITALIZATION_TYPE_WORDS");
    ENUM_ADD_DESCR(UITextField::AUTO_CAPITALIZATION_TYPE_SENTENCES, "AUTO_CAPITALIZATION_TYPE_SENTENCES");
    ENUM_ADD_DESCR(UITextField::AUTO_CAPITALIZATION_TYPE_ALL_CHARS, "AUTO_CAPITALIZATION_TYPE_ALL_CHARS");
};

ENUM_DECLARE(UITextField::eAutoCorrectionType)
{
    ENUM_ADD_DESCR(UITextField::AUTO_CORRECTION_TYPE_DEFAULT, "AUTO_CORRECTION_TYPE_DEFAULT");
    ENUM_ADD_DESCR(UITextField::AUTO_CORRECTION_TYPE_NO, "AUTO_CORRECTION_TYPE_NO");
    ENUM_ADD_DESCR(UITextField::AUTO_CORRECTION_TYPE_YES, "AUTO_CORRECTION_TYPE_YES");
};

ENUM_DECLARE(UITextField::eSpellCheckingType)
{
    ENUM_ADD_DESCR(UITextField::SPELL_CHECKING_TYPE_DEFAULT, "SPELL_CHECKING_TYPE_DEFAULT");
    ENUM_ADD_DESCR(UITextField::SPELL_CHECKING_TYPE_NO, "SPELL_CHECKING_TYPE_NO");
    ENUM_ADD_DESCR(UITextField::SPELL_CHECKING_TYPE_YES, "SPELL_CHECKING_TYPE_YES");
};

ENUM_DECLARE(UITextField::eKeyboardAppearanceType)
{
    ENUM_ADD_DESCR(UITextField::KEYBOARD_APPEARANCE_DEFAULT, "KEYBOARD_APPEARANCE_DEFAULT");
    ENUM_ADD_DESCR(UITextField::KEYBOARD_APPEARANCE_ALERT, "KEYBOARD_APPEARANCE_ALERT");
};

ENUM_DECLARE(UITextField::eKeyboardType)
{
    ENUM_ADD_DESCR(UITextField::KEYBOARD_TYPE_DEFAULT, "KEYBOARD_TYPE_DEFAULT");
    ENUM_ADD_DESCR(UITextField::KEYBOARD_TYPE_ASCII_CAPABLE, "KEYBOARD_TYPE_ASCII_CAPABLE");
    ENUM_ADD_DESCR(UITextField::KEYBOARD_TYPE_NUMBERS_AND_PUNCTUATION, "KEYBOARD_TYPE_NUMBERS_AND_PUNCTUATION");
    ENUM_ADD_DESCR(UITextField::KEYBOARD_TYPE_URL, "KEYBOARD_TYPE_URL");
    ENUM_ADD_DESCR(UITextField::KEYBOARD_TYPE_NUMBER_PAD, "KEYBOARD_TYPE_NUMBER_PAD");
    ENUM_ADD_DESCR(UITextField::KEYBOARD_TYPE_PHONE_PAD, "KEYBOARD_TYPE_PHONE_PAD");
    ENUM_ADD_DESCR(UITextField::KEYBOARD_TYPE_NAME_PHONE_PAD, "KEYBOARD_TYPE_NAME_PHONE_PAD");
    ENUM_ADD_DESCR(UITextField::KEYBOARD_TYPE_EMAIL_ADDRESS, "KEYBOARD_TYPE_EMAIL_ADDRESS");
    ENUM_ADD_DESCR(UITextField::KEYBOARD_TYPE_DECIMAL_PAD, "KEYBOARD_TYPE_DECIMAL_PAD");
    ENUM_ADD_DESCR(UITextField::KEYBOARD_TYPE_TWITTER, "KEYBOARD_TYPE_TWITTER");
};

ENUM_DECLARE(UITextField::eReturnKeyType)
{
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_DEFAULT, "RETURN_KEY_DEFAULT");
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_GO, "RETURN_KEY_GO");
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_GOOGLE, "RETURN_KEY_GOOGLE");
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_JOIN, "RETURN_KEY_JOIN");
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_NEXT, "RETURN_KEY_NEXT");
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_ROUTE, "RETURN_KEY_ROUTE");
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_SEARCH, "RETURN_KEY_SEARCH");
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_SEND, "RETURN_KEY_SEND");
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_YAHOO, "RETURN_KEY_YAHOO");
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_DONE, "RETURN_KEY_DONE");
    ENUM_ADD_DESCR(UITextField::RETURN_KEY_EMERGENCY_CALL, "RETURN_KEY_EMERGENCY_CALL");
};

ENUM_DECLARE(UITextField::eStartEditPolicy)
{
    ENUM_ADD_DESCR(UITextField::START_EDIT_WHEN_FOCUSED, "WhenFocused");
    ENUM_ADD_DESCR(UITextField::START_EDIT_BY_USER_REQUEST, "ByUserRequest");
};

ENUM_DECLARE(UITextField::eStopEditPolicy)
{
    ENUM_ADD_DESCR(UITextField::STOP_EDIT_WHEN_FOCUS_LOST, "WhenFocusLost");
    ENUM_ADD_DESCR(UITextField::STOP_EDIT_BY_USER_REQUEST, "ByUserRequest");
};

ENUM_DECLARE(UISizePolicyComponent::eSizePolicy)
{
    ENUM_ADD_DESCR(UISizePolicyComponent::IGNORE_SIZE, "Ignore");
    ENUM_ADD_DESCR(UISizePolicyComponent::FIXED_SIZE, "FixedSize");
    ENUM_ADD_DESCR(UISizePolicyComponent::PERCENT_OF_CHILDREN_SUM, "PercentOfChildrenSum");
    ENUM_ADD_DESCR(UISizePolicyComponent::PERCENT_OF_MAX_CHILD, "PercentOfMaxChild");
    ENUM_ADD_DESCR(UISizePolicyComponent::PERCENT_OF_FIRST_CHILD, "PercentOfFirstChild");
    ENUM_ADD_DESCR(UISizePolicyComponent::PERCENT_OF_LAST_CHILD, "PercentOfLastChild");
    ENUM_ADD_DESCR(UISizePolicyComponent::PERCENT_OF_CONTENT, "PercentOfContent");
    ENUM_ADD_DESCR(UISizePolicyComponent::PERCENT_OF_PARENT, "PercentOfParent");
    ENUM_ADD_DESCR(UISizePolicyComponent::FORMULA, "Formula");
};

ENUM_DECLARE(UILinearLayoutComponent::eOrientation)
{
    ENUM_ADD_DESCR(UILinearLayoutComponent::LEFT_TO_RIGHT, "LeftToRight");
    ENUM_ADD_DESCR(UILinearLayoutComponent::RIGHT_TO_LEFT, "RightToLeft");
    ENUM_ADD_DESCR(UILinearLayoutComponent::TOP_DOWN, "TopDown");
    ENUM_ADD_DESCR(UILinearLayoutComponent::BOTTOM_UP, "BottomUp");
};

ENUM_DECLARE(UIFlowLayoutComponent::eOrientation)
{
    ENUM_ADD_DESCR(UIFlowLayoutComponent::ORIENTATION_LEFT_TO_RIGHT, "LeftToRight");
    ENUM_ADD_DESCR(UIFlowLayoutComponent::ORIENTATION_RIGHT_TO_LEFT, "RightToLeft");
};

ENUM_DECLARE(eModifierKeys)
{
    ENUM_ADD_DESCR(static_cast<int>(eModifierKeys::SHIFT), "SHIFT");
    ENUM_ADD_DESCR(static_cast<int>(eModifierKeys::CONTROL), "CTRL");
    ENUM_ADD_DESCR(static_cast<int>(eModifierKeys::ALT), "ALT");
    ENUM_ADD_DESCR(static_cast<int>(eModifierKeys::COMMAND), "CMD");
};

ENUM_DECLARE(rhi::TextureAddrMode)
{
    ENUM_ADD_DESCR(rhi::TEXADDR_WRAP, "Wrap");
    ENUM_ADD_DESCR(rhi::TEXADDR_CLAMP, "Clamp");
    ENUM_ADD_DESCR(rhi::TEXADDR_MIRROR, "Mirror");
};

ENUM_DECLARE(rhi::TextureFilter)
{
    ENUM_ADD_DESCR(rhi::TEXFILTER_NEAREST, "Nearest");
    ENUM_ADD_DESCR(rhi::TEXFILTER_LINEAR, "Linear");
};

ENUM_DECLARE(rhi::TextureMipFilter)
{
    ENUM_ADD_DESCR(rhi::TEXMIPFILTER_NONE, "None");
    ENUM_ADD_DESCR(rhi::TEXMIPFILTER_NEAREST, "Nearest");
    ENUM_ADD_DESCR(rhi::TEXMIPFILTER_LINEAR, "Linear");
};

ENUM_DECLARE(rhi::PrimitiveType)
{
    ENUM_ADD(rhi::PRIMITIVE_TRIANGLELIST);
    ENUM_ADD(rhi::PRIMITIVE_TRIANGLESTRIP);
    ENUM_ADD(rhi::PRIMITIVE_LINELIST);
};

ENUM_DECLARE(UIWebView::eDataDetectorType)
{
    ENUM_ADD_DESCR(UIWebView::DATA_DETECTOR_PHONE_NUMBERS, "PhoneNumbers");
    ENUM_ADD_DESCR(UIWebView::DATA_DETECTOR_LINKS, "Links");
    ENUM_ADD_DESCR(UIWebView::DATA_DETECTOR_ADDRESSES, "Addresses");
    ENUM_ADD_DESCR(UIWebView::DATA_DETECTOR_CALENDAR_EVENTS, "CalendarEvents");
};

ENUM_DECLARE(eGradientBlendMode)
{
    ENUM_ADD_DESCR(GRADIENT_MULTIPLY, "Multiply");
    ENUM_ADD_DESCR(GRADIENT_BLEND, "Alpha blend");
    ENUM_ADD_DESCR(GRADIENT_ADD, "Additive");
    ENUM_ADD_DESCR(GRADIENT_SCREEN, "Screen");
    ENUM_ADD_DESCR(GRADIENT_OVERLAY, "Overlay");
};

ENUM_DECLARE(BillboardRenderObject::BillboardType)
{
    ENUM_ADD_DESCR(BillboardRenderObject::BILLBOARD_SPHERICAL, "Spherical");
    ENUM_ADD_DESCR(BillboardRenderObject::BILLBOARD_CYLINDRICAL, "Cylindrical");
}

ENUM_DECLARE(GeoDecalManager::Mapping)
{
    ENUM_ADD_DESCR(GeoDecalManager::Mapping::PLANAR, "Planar");
    ENUM_ADD_DESCR(GeoDecalManager::Mapping::SPHERICAL, "Spherical");
    ENUM_ADD_DESCR(GeoDecalManager::Mapping::CYLINDRICAL, "Cylindrical");
}
