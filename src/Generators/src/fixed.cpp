#include "fixed.h"

void GeneratorFixed::setParticle(std::string name)
{
  particle=name;
  mass=getMass(name);
}

void GeneratorFixed::setTheta(double t)
{
  theta=t;
}

void GeneratorFixed::setPhi(double p)
{
  phi = p;
}

void GeneratorFixed::setMomentum(double mom)
{
  momentum=mom;
}

int GeneratorFixed::generateEvent(GeneratorEvent *ev)
{
  ev->weight=1;
  GeneratorParticle p;
  p.particle=particle;
  p.momentum.SetXYZM(momentum*sin(theta)*cos(phi),momentum*sin(theta)*sin(phi),momentum*cos(theta),mass);
  ev->particles.push_back(p);
  return 1;
}
