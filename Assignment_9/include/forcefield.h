class ForceField
{
public:
    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const = 0;
};
class GravityForceField : public ForceField
{
};

class ConstantForceField : public ForceField
{
};
class RadialForceField : public ForceField
{
};
class VerticalForceField : public ForceField
{
};