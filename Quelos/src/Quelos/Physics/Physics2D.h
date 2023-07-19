#pragma once

#include "Quelos/Scene/Components.h"

// Box2D
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

namespace Quelos
{
	namespace Utils
	{
		inline b2BodyType Rigidbody2DTypeToBox2D(Rigidbody2DComponent::BodyType type)
		{
			switch (type)
			{
			case Quelos::Rigidbody2DComponent::BodyType::Static:		return b2_staticBody;
			case Quelos::Rigidbody2DComponent::BodyType::Dynamic:		return b2_dynamicBody;
			case Quelos::Rigidbody2DComponent::BodyType::Kinematic:	return b2_kinematicBody;
			}

			QS_CORE_ASSERT(false, "Unknow BodyType");
			return b2_staticBody;
		}
	}
}
