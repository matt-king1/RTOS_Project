/*
 * tests.c
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */

#include "PhysicsEngine/engine.h"

extern int GRAVITY_PIXELS;

bool testPlatformUpdate(void)
{
  struct PlatformData plat_data = (struct PlatformData){
      .x = 64,
      .vx = 2,
      .ax = 1
  };

  updatePlatform(&plat_data);
  return (plat_data.x < (64 + (2 * 2 * PHYSICS_DELTA)) && plat_data.x > 64) && (plat_data.vx < (2 + (2 * PHYSICS_DELTA)) && plat_data.vx > 2);
}

bool testHMsUpdate(void)
{
  struct HoltzmanData hms[] = {
      (struct HoltzmanData){
        .x = 64,
        .y = 64,
        .vx = 20,
        .vy = 5,
        .mode = 0
      },
  };
  updateHMs(hms);

  return (hms[0].vx == 20) &&
  (hms[0].vy == 5 + (GRAVITY_PIXELS * PHYSICS_DELTA)) &&
  (hms[0].x == 64 + (20 * PHYSICS_DELTA)) &&
  (hms[0].y == 64 + (5 * PHYSICS_DELTA));
}

bool testHMsRepelNoShield()
{
  struct HoltzmanData hms[] = {(struct HoltzmanData){
        .x = 5,
        .y = PLATFORM_Y + 1,
        .vx = 0,
        .vy = 30,
        .mode = 0
  }};

  struct PlatformData plat_data = (struct PlatformData){
        .x = 5,
        .vx = 0,
        .ax = 0
  };

  struct ShieldState shield = (struct ShieldState) {.active = false, .recharging = false};

  checkHMs(hms, &plat_data, &shield);
  return (hms[0].vy == 30 * PASSIVE_KINETIC_REDUCTION); //hm was repelled at correct velocity
}

bool testHMsRepelShield()
{
  struct HoltzmanData hms[] = {(struct HoltzmanData){
    .x = 5,
        .y = PLATFORM_Y,
        .vx = 0,
        .vy = 30,
        .mode = 0
  }};

  struct PlatformData plat_data = (struct PlatformData){
    .x = 5,
        .vx = 0,
        .ax = 0
  };

  struct ShieldState shield = (struct ShieldState) {.active = true, .recharging = false};
  checkHMs(hms, &plat_data, &shield);
  return(hms[0].vy == 30 * ACTIVE_KINETIC_GAIN);
}

bool testHMsUpdateWall()
{
  struct HoltzmanData hms[] = {(struct HoltzmanData){
    .x = 129,
    .y = 64,
    .vx = 20,
    .vy = 0,
    .mode = 0
  }};

  updateHMs(hms);
  return(hms[0].vx == -20);
}


void runTests()
{
  int count = 0;
  if(testPlatformUpdate()) count++;
  else printf("Faled platform update test\n");
  if(testHMsUpdate()) count++;
  else printf("Faled HMs update test\n");
  if(testHMsRepelNoShield()) count++;
  else printf("Failed repel HMs without shield test\n");
  if(testHMsRepelShield()) count++;
  else printf("Failed repel HMs with shield test\n");
  if(testHMsUpdateWall()) count++;
  else printf("Failed HM wall collision test\n");

  printf("Passed %d / 5 tests", count);
}
