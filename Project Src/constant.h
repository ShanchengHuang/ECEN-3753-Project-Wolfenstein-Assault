Data Structure Version = 1 (for the data structure shown below)
o TauPhysics [ms] = 50
o TauDisplay [ms] = 150
o TauSlider [ms] = 100
o CanyonSize [cm] = 100000
o Wolfenstein
 CastleHeight[cm] = 5000
 FoundationHitsRequired [-] = 2
 FoundationDepth[cm] = 5000
o SatchelCharges
 LimitingMethod [enum: AlwaysOne=0, MaxInFlight=1, PeriodicThrowTime=2] = 0
 DisplayDiameter [pixels] = 10
 Union (usage based on LimitingMethod):
 N/A, TauThrow, TauThrow
 Union (usage based on LimitingMethod):
 N/A, MaxNumInFlight, N/A
o Platform
 MaxForce [N] = 20000000
 Mass [kg] = 100
 Length [cm] = 10000
 MaxPlatformBounceSpeed [cm/s] = 50000
o Shield
 EffectiveRange[cm] = 15000
 ActivationEnergy [kJ] = 30000
o RailGun
 ElevationAngle [mrad] = 800
 ShotMass[kg] = 50
 ShotDisplayDiameter[pixels] = 5
o Generator
 EnergyStorage [kJ] = 50000
 Power [kW] = 20000