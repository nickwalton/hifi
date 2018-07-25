//
//  Created by Sam Gondelman 10/20/2017
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "PickScriptingInterface.h"

#include <QVariant>
#include "GLMHelpers.h"

#include <PickManager.h>

#include "StaticRayPick.h"
#include "JointRayPick.h"
#include "MouseRayPick.h"
#include "StylusPick.h"
#include "CollisionPick.h"

#include <ScriptEngine.h>

unsigned int PickScriptingInterface::createPick(const PickQuery::PickType type, const QVariant& properties) {
    switch (type) {
        case PickQuery::PickType::Ray:
            return createRayPick(properties);
        case PickQuery::PickType::Stylus:
            return createStylusPick(properties);
        case PickQuery::PickType::Collision:
            return createCollisionPick(properties);
        default:
            return PickManager::INVALID_PICK_ID;
    }
}

/**jsdoc
 * A set of properties that can be passed to {@link Picks.createPick} to create a new Ray Pick.
 * @typedef {object} Picks.RayPickProperties
 * @property {boolean} [enabled=false] If this Pick should start enabled or not.  Disabled Picks do not updated their pick results.
 * @property {number} [filter=Picks.PICK_NOTHING] The filter for this Pick to use, constructed using filter flags combined using bitwise OR.
 * @property {number} [maxDistance=0.0] The max distance at which this Pick will intersect.  0.0 = no max.  < 0.0 is invalid.
 * @property {string} [joint] Only for Joint or Mouse Ray Picks.  If "Mouse", it will create a Ray Pick that follows the system mouse, in desktop or HMD.
 *   If "Avatar", it will create a Joint Ray Pick that follows your avatar's head.  Otherwise, it will create a Joint Ray Pick that follows the given joint, if it
 *   exists on your current avatar.
 * @property {Vec3} [posOffset=Vec3.ZERO] Only for Joint Ray Picks.  A local joint position offset, in meters.  x = upward, y = forward, z = lateral
 * @property {Vec3} [dirOffset=Vec3.UP] Only for Joint Ray Picks.  A local joint direction offset.  x = upward, y = forward, z = lateral
 * @property {Vec3} [position] Only for Static Ray Picks.  The world-space origin of the ray.
 * @property {Vec3} [direction=-Vec3.UP] Only for Static Ray Picks.  The world-space direction of the ray.
 */
unsigned int PickScriptingInterface::createRayPick(const QVariant& properties) {
    QVariantMap propMap = properties.toMap();

    bool enabled = false;
    if (propMap["enabled"].isValid()) {
        enabled = propMap["enabled"].toBool();
    }

    PickFilter filter = PickFilter();
    if (propMap["filter"].isValid()) {
        filter = PickFilter(propMap["filter"].toUInt());
    }

    float maxDistance = 0.0f;
    if (propMap["maxDistance"].isValid()) {
        maxDistance = propMap["maxDistance"].toFloat();
    }

    if (propMap["joint"].isValid()) {
        std::string jointName = propMap["joint"].toString().toStdString();

        if (jointName != "Mouse") {
            // x = upward, y = forward, z = lateral
            glm::vec3 posOffset = Vectors::ZERO;
            if (propMap["posOffset"].isValid()) {
                posOffset = vec3FromVariant(propMap["posOffset"]);
            }

            glm::vec3 dirOffset = Vectors::UP;
            if (propMap["dirOffset"].isValid()) {
                dirOffset = vec3FromVariant(propMap["dirOffset"]);
            }

            return DependencyManager::get<PickManager>()->addPick(PickQuery::Ray, std::make_shared<JointRayPick>(jointName, posOffset, dirOffset, filter, maxDistance, enabled));

        } else {
            return DependencyManager::get<PickManager>()->addPick(PickQuery::Ray, std::make_shared<MouseRayPick>(filter, maxDistance, enabled));
        }
    } else if (propMap["position"].isValid()) {
        glm::vec3 position = vec3FromVariant(propMap["position"]);

        glm::vec3 direction = -Vectors::UP;
        if (propMap["direction"].isValid()) {
            direction = vec3FromVariant(propMap["direction"]);
        }

        return DependencyManager::get<PickManager>()->addPick(PickQuery::Ray, std::make_shared<StaticRayPick>(position, direction, filter, maxDistance, enabled));
    }

    return PickManager::INVALID_PICK_ID;
}

/**jsdoc
 * A set of properties that can be passed to {@link Picks.createPick} to create a new Stylus Pick.
 * @typedef {object} Picks.StylusPickProperties
 * @property {number} [hand=-1] An integer.  0 == left, 1 == right.  Invalid otherwise.
 * @property {boolean} [enabled=false] If this Pick should start enabled or not.  Disabled Picks do not updated their pick results.
 * @property {number} [filter=Picks.PICK_NOTHING] The filter for this Pick to use, constructed using filter flags combined using bitwise OR.
 * @property {number} [maxDistance=0.0] The max distance at which this Pick will intersect.  0.0 = no max.  < 0.0 is invalid.
 */
unsigned int PickScriptingInterface::createStylusPick(const QVariant& properties) {
    QVariantMap propMap = properties.toMap();

    bilateral::Side side = bilateral::Side::Invalid;
    {
        QVariant handVar = propMap["hand"];
        if (handVar.isValid()) {
            side = bilateral::side(handVar.toInt());
        }
    }

    bool enabled = false;
    if (propMap["enabled"].isValid()) {
        enabled = propMap["enabled"].toBool();
    }

    PickFilter filter = PickFilter();
    if (propMap["filter"].isValid()) {
        filter = PickFilter(propMap["filter"].toUInt());
    }

    float maxDistance = 0.0f;
    if (propMap["maxDistance"].isValid()) {
        maxDistance = propMap["maxDistance"].toFloat();
    }

    return DependencyManager::get<PickManager>()->addPick(PickQuery::Stylus, std::make_shared<StylusPick>(side, filter, maxDistance, enabled));
}

/**jsdoc
* A Shape defines a physical volume.
*
* @typedef {object} Shape
* @property {ShapeType} shapeType The type of shape to use.
* @property {string} modelURL - If shapeType is one of: "compound", "simple-hull", "simple-compound", or "static-mesh", this defines the model to load to generate the collision volume.
* @property {Vec3} dimensions - The size to scale the shape to.
*/

/**jsdoc
* A set of properties that can be passed to {@link Picks.createPick} to create a new Collision Pick.

* @typedef {object} Picks.CollisionPickProperties
* @property {Shape} shape - The information about the collision region's size and shape.
* @property {Vec3} position - The position of the collision region.
* @property {Quat} orientation - The orientation of the collision region.
*/
unsigned int PickScriptingInterface::createCollisionPick(const QVariant& properties) {
    QVariantMap propMap = properties.toMap();

    bool enabled = false;
    if (propMap["enabled"].isValid()) {
        enabled = propMap["enabled"].toBool();
    }

    PickFilter filter = PickFilter();
    if (propMap["filter"].isValid()) {
        filter = PickFilter(propMap["filter"].toUInt());
    }

    float maxDistance = 0.0f;
    if (propMap["maxDistance"].isValid()) {
        maxDistance = propMap["maxDistance"].toFloat();
    }

    CollisionRegion collisionRegion(propMap);

    return DependencyManager::get<PickManager>()->addPick(PickQuery::Collision, std::make_shared<CollisionPick>(filter, maxDistance, enabled, collisionRegion, _physicsEngine));
}

void PickScriptingInterface::enablePick(unsigned int uid) {
    DependencyManager::get<PickManager>()->enablePick(uid);
}

void PickScriptingInterface::disablePick(unsigned int uid) {
    DependencyManager::get<PickManager>()->disablePick(uid);
}

void PickScriptingInterface::removePick(unsigned int uid) {
    DependencyManager::get<PickManager>()->removePick(uid);
}

QVariantMap PickScriptingInterface::getPrevPickResult(unsigned int uid) {
    QVariantMap result;
    auto pickResult = DependencyManager::get<PickManager>()->getPrevPickResult(uid);
    if (pickResult) {
        result = pickResult->toVariantMap();
    }
    return result;
}

void PickScriptingInterface::setPrecisionPicking(unsigned int uid, bool precisionPicking) {
    DependencyManager::get<PickManager>()->setPrecisionPicking(uid, precisionPicking);
}

void PickScriptingInterface::setIgnoreItems(unsigned int uid, const QScriptValue& ignoreItems) {
    DependencyManager::get<PickManager>()->setIgnoreItems(uid, qVectorQUuidFromScriptValue(ignoreItems));
}

void PickScriptingInterface::setIncludeItems(unsigned int uid, const QScriptValue& includeItems) {
    DependencyManager::get<PickManager>()->setIncludeItems(uid, qVectorQUuidFromScriptValue(includeItems));
}

bool PickScriptingInterface::isLeftHand(unsigned int uid) {
    return DependencyManager::get<PickManager>()->isLeftHand(uid);
}

bool PickScriptingInterface::isRightHand(unsigned int uid) {
    return DependencyManager::get<PickManager>()->isRightHand(uid);
}

bool PickScriptingInterface::isMouse(unsigned int uid) {
    return DependencyManager::get<PickManager>()->isMouse(uid);
}

QScriptValue pickTypesToScriptValue(QScriptEngine* engine, const PickQuery::PickType& pickType) {
    return pickType;
}

void pickTypesFromScriptValue(const QScriptValue& object, PickQuery::PickType& pickType) {
    pickType = static_cast<PickQuery::PickType>(object.toUInt16());
}

void PickScriptingInterface::registerMetaTypes(QScriptEngine* engine) {
    QScriptValue pickTypes = engine->newObject();
    auto metaEnum = QMetaEnum::fromType<PickQuery::PickType>();
    for (int i = 0; i < PickQuery::PickType::NUM_PICK_TYPES; ++i) {
        pickTypes.setProperty(metaEnum.key(i), metaEnum.value(i));
    }
    engine->globalObject().setProperty("PickType", pickTypes);

    qScriptRegisterMetaType(engine, pickTypesToScriptValue, pickTypesFromScriptValue);
}

unsigned int PickScriptingInterface::getPerFrameTimeBudget() const {
    return DependencyManager::get<PickManager>()->getPerFrameTimeBudget();
}

void PickScriptingInterface::setPerFrameTimeBudget(unsigned int numUsecs) {
    DependencyManager::get<PickManager>()->setPerFrameTimeBudget(numUsecs);
}
