// HyperRogue
// This file contains the routines to convert HyperRogue's old vector graphics into 3D models

// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

#include "earcut.hpp"

namespace hr {

ld eyepos;

#if MAXMDIM >= 4

#define S (scalefactor / 0.805578)
#define SH (scalefactor / 0.805578 * geom3::height_width / 1.5)

#define revZ (WDIM == 2 ? -1 : 1)

hyperpoint shcenter;

vector<hyperpoint> get_shape(hpcshape sh) {
  vector<hyperpoint> res;
  for(int i=sh.s; i<sh.e-1; i++) res.push_back(hpc[i]);
  return res;  
  }

hyperpoint get_center(const vector<hyperpoint>& vh) {
  hyperpoint h = Hypc;
  using namespace hyperpoint_vec;
  for(auto h1: vh) h = h + h1;
  return normalize(h);
  }

ld zc(ld z) { 
  if(WDIM == 2 && GDIM == 3)
    return geom3::lev_to_factor(geom3::human_height * z);
  return geom3::human_height * (z - 0.5); 
  }

transmatrix zpush(ld z) {
  return cpush(2, z);
  }

void add_cone(ld z0, const vector<hyperpoint>& vh, ld z1) {
  last->flags |= POLY_TRIANGLES;
  for(int i=0; i<isize(vh); i++) {
    hpcpush(zpush(z0) * vh[i]);
    hpcpush(zpush(z0) * vh[(i+1) % isize(vh)]);
    hpcpush(zpush(z1) * shcenter);
    }
  }

void add_prism_sync(ld z0, vector<hyperpoint> vh0, ld z1, vector<hyperpoint> vh1) {
  last->flags |= POLY_TRIANGLES;
  for(int i=0; i<isize(vh0); i++) {
    int i1 = (i+1) % isize(vh0);
    hpcpush(zpush(z0) * vh0[i]);
    hpcpush(zpush(z1) * vh1[i]);
    hpcpush(zpush(z0) * vh0[i1]);
    hpcpush(zpush(z1) * vh1[i]);
    hpcpush(zpush(z0) * vh0[i1]);
    hpcpush(zpush(z1) * vh1[i1]);
    }
  }

void add_prism(ld z0, vector<hyperpoint> vh0, ld z1, vector<hyperpoint> vh1) {
  last->flags |= POLY_TRIANGLES;
  
  struct mixed {
    ld angle;
    int owner;
    hyperpoint h;
    mixed(ld a, int o, hyperpoint _h) : angle(a), owner(o), h(_h) {}
    };
  
  transmatrix T0 = gpushxto0(get_center(vh0));
  transmatrix T1 = gpushxto0(get_center(vh1));
  
  vector<mixed> pairs;
  for(auto h: vh0) pairs.emplace_back(atan2(T0*h), 0, h);
  for(auto h: vh1) pairs.emplace_back(atan2(T1*h), 1, h);
  sort(pairs.begin(), pairs.end(), [&] (const mixed p, const mixed q) { return p.angle < q.angle; });
  
  hyperpoint lasts[2];
  for(auto pp: pairs) lasts[pp.owner] = pp.h;
  
  for(auto pp: pairs) {
    int id = pp.owner;
    hpcpush(zpush(z0) * lasts[0]);
    hpcpush(zpush(z1) * lasts[1]);
    hpcpush(zpush(id == 0 ? z0 : z1) * pp.h);
    lasts[id] = pp.h;
    }
  }
  
void shift_last(ld z) {
  for(int i=last->s; i<isize(hpc); i++) hpc[i] = zshift(hpc[i], z);
  }

void shift_shape(hpcshape& sh, ld z) {
  for(int i=sh.s; i<sh.e; i++) hpc[i] = zshift(hpc[i], z);
  }

extern
hpcshape 
  shSemiFloorSide[SIDEPARS],
  shBFloor[2],
  shWave[8][2],  
  shCircleFloor,
  shBarrel,
  shWall[2], shMineMark[2], shBigMineMark[2], shFan,
  shZebra[5],
  shSwitchDisk,
  shTower[11],
  shEmeraldFloor[6],
  shSemiFeatherFloor[2], 
  shSemiFloor[2], shSemiBFloor[2], shSemiFloorShadow,
  shMercuryBridge[2],
  shTriheptaSpecial[14], 
  shCross, shGiantStar[2], shLake, shMirror,
  shHalfFloor[3], shHalfMirror[3],
  shGem[2], shStar, shDisk, shDiskT, shDiskS, shDiskM, shDiskSq, shRing,   
  shTinyBird, shTinyShark,
  shEgg,
  shSpikedRing, shTargetRing, shSawRing, shGearRing, shPeaceRing, shHeptaRing,
  shSpearRing, shLoveRing,
  shDaisy, shTriangle, shNecro, shStatue, shKey, shWindArrow,
  shGun,
  shFigurine, shTreat,
  shElementalShard,
  // shBranch, 
  shIBranch, shTentacle, shTentacleX, shILeaf[2], 
  shMovestar,
  shWolf, shYeti, shDemon, shGDemon, shEagle, shGargoyleWings, shGargoyleBody,
  shFoxTail1, shFoxTail2,
  shDogBody, shDogHead, shDogFrontLeg, shDogRearLeg, shDogFrontPaw, shDogRearPaw,
  shDogTorso,
  shHawk,
  shCatBody, shCatLegs, shCatHead, shFamiliarHead, shFamiliarEye,
  shWolf1, shWolf2, shWolf3,
  shDogStripes,
  shPBody, shPSword, shPKnife,
  shFerocityM, shFerocityF, 
  shHumanFoot, shHumanLeg, shHumanGroin, shHumanNeck, shSkeletalFoot, shYetiFoot,
  shMagicSword, shMagicShovel, shSeaTentacle, shKrakenHead, shKrakenEye, shKrakenEye2,
  shArrow,
  shPHead, shPFace, shGolemhead, shHood, shArmor, 
  shAztecHead, shAztecCap,
  shSabre, shTurban1, shTurban2, shVikingHelmet, shRaiderHelmet, shRaiderArmor, shRaiderBody, shRaiderShirt,
  shWestHat1, shWestHat2, shGunInHand,
  shKnightArmor, shKnightCloak, shWightCloak,
  shGhost, shEyes, shSlime, shJelly, shJoint, shWormHead, shTentHead, shShark, shWormSegment, shSmallWormSegment, shWormTail, shSmallWormTail,
  shMiniGhost, shMiniEyes,
  shHedgehogBlade, shHedgehogBladePlayer,
  shWolfBody, shWolfHead, shWolfLegs, shWolfEyes,
  shWolfFrontLeg, shWolfRearLeg, shWolfFrontPaw, shWolfRearPaw,
  shFemaleBody, shFemaleHair, shFemaleDress, shWitchDress,
  shWitchHair, shBeautyHair, shFlowerHair, shFlowerHand, shSuspenders, shTrophy,
  shBugBody, shBugArmor, shBugLeg, shBugAntenna,
  shPickAxe, shPike, shFlailBall, shFlailTrunk, shFlailChain, shHammerHead,
  shBook, shBookCover, shGrail,
  shBoatOuter, shBoatInner, shCompass1, shCompass2, shCompass3,
  shKnife, shTongue, shFlailMissile, shTrapArrow,
  shPirateHook, shPirateHood, shEyepatch, shPirateX,
  // shScratch, 
  shHeptaMarker, shSnowball, 
  shSkeletonBody, shSkull, shSkullEyes, shFatBody, shWaterElemental,
  shPalaceGate, shFishTail,
  shMouse, shMouseLegs, shMouseEyes,
  shPrincessDress, shPrinceDress,
  shWizardCape1, shWizardCape2,
  shBigCarpet1, shBigCarpet2, shBigCarpet3,
  shGoatHead, shRose, shThorns,
  shRatHead, shRatTail, shRatEyes, shRatCape1, shRatCape2,
  shWizardHat1, shWizardHat2,
  shTortoise[13][6],
  shDragonLegs, shDragonTail, shDragonHead, shDragonSegment, shDragonNostril, 
  shDragonWings, 
  shSolidBranch, shWeakBranch, shBead0, shBead1,
  shBatWings, shBatBody, shBatMouth, shBatFang, shBatEye,
  shParticle[16], shAsteroid[8],
  shReptile[5][4],
  shReptileBody, shReptileHead, shReptileFrontFoot, shReptileRearFoot,
  shReptileFrontLeg, shReptileRearLeg, shReptileTail, shReptileEye,

  shTrylobite, shTrylobiteHead, shTrylobiteBody,
  shTrylobiteFrontLeg, shTrylobiteRearLeg, shTrylobiteFrontClaw, shTrylobiteRearClaw,
  
  shBullBody, shBullHead, shBullHorn, shBullRearHoof, shBullFrontHoof,
  
  shButterflyBody, shButterflyWing, shGadflyBody, shGadflyWing, shGadflyEye,

  shTerraArmor1, shTerraArmor2, shTerraArmor3, shTerraHead, shTerraFace, 
  shJiangShi, shJiangShiDress, shJiangShiCap1, shJiangShiCap2,
  
  shAsymmetric,
  
  shPBodyOnly, shPBodyArm, shPBodyHand, shPHeadOnly,
  
  shDodeca;


extern renderbuffer *floor_textures;

basic_textureinfo models_texture;

void add_texture(hpcshape& sh) {
  if(!floor_textures) return;
  auto& utt = models_texture;
  sh.tinf = &utt;
  sh.texture_offset = isize(utt.tvertices);
  for(int i=sh.s; i<isize(hpc); i++) {
    hyperpoint h = hpc[i];
    ld rad = hypot_d(3, h);
    ld factor = 0.50 + (0.17 * h[2] + 0.13 * h[1] + 0.15 * h[0]) / rad;
    utt.tvertices.push_back(glhr::makevertex(0, factor, 0));
    }
  }

vector<hyperpoint> scaleshape(const vector<hyperpoint>& vh, ld s) {
  vector<hyperpoint> res;
  using namespace hyperpoint_vec;
  for(hyperpoint h: vh) res.push_back(normalize(h * s + shcenter * (1-s)));
  return res;
  }

void make_ha_3d(hpcshape& sh, bool isarmor, ld scale) {
  shcenter = C0;

  auto groin = get_shape(shHumanGroin);
  auto body = get_shape(shPBodyOnly);
  auto neck = get_shape(shHumanNeck);
  auto hand = get_shape(shPBodyHand);
  auto arm = get_shape(shPBodyArm);
  groin = scaleshape(groin, scale);
  neck = scaleshape(neck, scale);

  auto fullbody = get_shape(sh);
  
  auto body7 = body[7];
  auto body26 = body[26];
  body.clear();

  bool foundplus = false, foundminus = false;
  for(hyperpoint h: fullbody) {
    if(h[1] > 0.14 * S) {
      if(foundplus) ;
      else foundplus = true, body.push_back(body7);
      }
    else if(h[1] < -0.14 * S) {
      if(foundminus) ;
      else foundminus = true, body.push_back(body26);
      }
    else body.push_back(h);
    }
  
  auto arm8 = arm[8];
  bool armused = false;
  arm.clear();  
  for(hyperpoint h: fullbody) {
    if(h[1] < 0.08 * S) ;
    else if(h[0] > -0.03 * S) {
      if(armused) ;
      else armused = true, arm.push_back(arm8);
      }
    else arm.push_back(h);
    }
  
  auto hand0 = hand[0];
  hand.clear();
  hand.push_back(hand0);
  for(hyperpoint h: fullbody) {
    if(h[1] + h[0] > 0.13 * S) hand.push_back(h);
    }

  bshape(sh, PPR::MONSTER_BODY);
  add_cone(zc(0.4), groin, zc(0.36));
  add_prism_sync(zc(0.4), groin, zc(0.6), groin);
  add_prism(zc(0.6), groin, zc(0.7), body);
  add_prism(zc(0.7), body, zc(0.8), neck);
  
  add_cone(zc(0.8), neck, zc(0.83));

  int at0 = isize(hpc);
  ld h = geom3::human_height;
  
  if(isize(arm) > 3) {
    shcenter = get_center(arm);
    int arm0 = isize(hpc);
    add_prism_sync(geom3::BODY - h*.03, arm, geom3::BODY + h*.03, arm);
    add_cone(geom3::BODY + h*.03, arm, geom3::BODY + h*.05);
    add_cone(geom3::BODY - h*.03, arm, geom3::BODY - h*.05);
    int arm1 = isize(hpc);
    for(int i=arm0; i<arm1; i++) {
      hyperpoint h = hpc[i];
      ld zl = asinh(h[2]);
      h = zpush(-zl) * h;
      ld rad = hdist0(h);
      rad = (rad - 0.1124*S) / (0.2804*S - 0.1124*S);
      rad = 1 - rad;
      rad *= zc(0.7) - geom3::BODY;
      hpc[i] = zpush(rad) * hpc[i];
      }
    }
   // 0.2804 - keep
   // 0.1124 - move
   
  if(isize(hand) > 3) {
    shcenter = get_center(hand);
    add_cone(geom3::BODY, hand, geom3::BODY + 0.05 * geom3::human_height);
    add_cone(geom3::BODY, hand, geom3::BODY - 0.05 * geom3::human_height);
    }

  int at1 = isize(hpc);
  for(int i=at0; i<at1; i++) hpc.push_back(Mirror * hpc[i]);  
  
  add_texture(shPBody);
  shift_last(-geom3::BODY);
  }

/*
void make_humanoid_3d(hpcshape& sh) { make_ha_3d(sh, false, 0.90); }
void make_armor_3d(hpcshape& sh, ld scale = 1) { make_ha_3d(sh, true, scale); }
*/

void make_humanoid_3d(hpcshape& sh) { make_ha_3d(sh, false, 1); }

void addtri(array<hyperpoint, 3> hs, int kind) {
  ld ds[3];
  ds[0] = hdist(hs[0], hs[1]);
  ds[1] = hdist(hs[1], hs[2]);
  ds[2] = hdist(hs[2], hs[0]);
  ld maxds = 0;
  for(int i=0; i<3; i++) maxds = max(ds[i], maxds) - 1e-3;
  
  if(maxds > 0.02*S) for(int i=0; i<3; i++) {
    int j = (i+1) % 3;
    int k = (j+1) % 3;
    if(hdist(hs[i], hs[j]) > maxds) {
      auto hm = mid(hs[i], hs[j]);
      addtri(make_array(hm, hs[i], hs[k]), kind);
      addtri(make_array(hm, hs[j], hs[k]), kind);
      return;
      }
    }
  
  if(kind) {
    array<hyperpoint, 3> ht;
    ld hsh[3];
    ld shi[3];
    bool ok = true;
    for(int s=0; s<3; s++) {
      hs[s] = normalize(hs[s]);
      hyperpoint h = hs[s];
      ld zz = zc(0.78);
      hsh[s] = abs(h[1]);
      zz -= h[1] * h[1] / 0.14 / 0.14 * 0.01 / S / S * SH;
      zz -= h[0] * h[0] / 0.10 / 0.10 * 0.01 / S / S * SH;
      if(abs(h[1]) > 0.14*S) ok = false, zz -= revZ * (abs(h[1])/S - 0.14) * SH;
      if(abs(h[0]) > 0.08*S) ok = false, zz -= revZ * (abs(h[0])/S - 0.08) * (abs(h[0])/S - 0.08) * 25 * SH;
      h = normalize(h);
      ht[s] = zpush(zz) * h;
      if(hsh[s] < 0.1*S) shi[s] = 0.5;
      else if(hsh[s] < 0.12*S) shi[s] = 0.1 + 0.4 * (hsh[s]/S - 0.1) / (0.12 - 0.1);
      else shi[s] = 0.1;
      }
    if(ok && kind == 1) {
      array<array<hyperpoint, 3>, 6> htx;
      for(int i=0; i<6; i++) htx[i] = ht;

      for(int i=0; i<3; i++) {
        htx[0][i][0] *= 0.7; htx[0][i][1] *= 0.7;
        htx[1][i][0] *= 1.2; htx[1][i][1] *= 1.7;
        htx[2][i][1] *= 1.7;
        htx[4][i][0] = htx[4][i][0] * 0.4 + scalefactor * 0.1;
        htx[5][i][0] = htx[5][i][0] * 0.3 + scalefactor * 0.1;
        for(int a=0; a<6; a++) htx[a][i] = hpxy3(htx[a][i][0], htx[a][i][1], htx[a][i][2]);
        }
      ld levels[6] = {0, 0.125, 0.125, 0.250, 0.375, 0.5};
      for(int a=0; a<6; a++) for(int i=0; i<3; i++) 
        htx[a][i] = zpush(-min(shi[i], levels[a]) * geom3::human_height * revZ) * htx[a][i];
      
      hpcpush(htx[0][0]);
      hpcpush(htx[0][1]);
      hpcpush(htx[0][2]);

      for(int a=0; a<5; a++) for(int i=0; i<3; i++) {
        int j = (i+1) % 3;
        int b = a+1;
        hpcpush(htx[a][i]);
        hpcpush(htx[a][j]);
        hpcpush(htx[b][i]);
        hpcpush(htx[a][j]);
        hpcpush(htx[b][i]);
        hpcpush(htx[b][j]);
        }
      }
    else 
      hpcpush(ht[0]), hpcpush(ht[1]), hpcpush(ht[2]);
    }
  else {
    for(int s=0; s<3; s++) {
      hyperpoint h = hs[s];
      ld zz = zc(eyepos);
      if(h[0] < -0.05*S) zz += revZ * (h[0]/S + 0.05) * SH;
      if(hdist0(h) <= 0.0501*S) {
        zz += revZ * sqrt(0.0026 - pow(hdist0(h)/S, 2)) * SH;
        }
      hpcpush(zpush(zz) * h);
      }
    }
  }

void make_armor_3d(hpcshape& sh, int kind = 1) { 

  auto body = get_shape(sh);
  vector<vector<array<ld, 2> >> pts(2);
  
  for(hyperpoint h: body) {
    array<ld, 2> p;
    p[0] = h[0] / h[3];
    p[1] = h[1] / h[3];
    pts[0].emplace_back(p);
    }
  
  bshape(sh, sh.prio);
  
  vector<int> indices = mapbox::earcut<int> (pts);
  
  last->flags |= POLY_TRIANGLES;

  last->flags |= POLY_TRIANGLES;
  for(int k=0; k<isize(indices); k+=3) {
    addtri(make_array(body[indices[k]], body[indices[k+1]], body[indices[k+2]]), kind);
    }
  
  add_texture(sh);
  if(&sh == &shHood || &sh == &shWightCloak || &sh == &shArmor)
    shift_last(-geom3::HEAD);
  else
    shift_last(-geom3::BODY);
  }

void make_foot_3d(hpcshape& sh) {
  auto foot = get_shape(sh);
  auto leg = get_shape(shHumanLeg);
  auto leg5 = scaleshape(leg, 0.8);

  bshape(sh, PPR::MONSTER_BODY);
  shcenter = get_center(leg);
  add_cone(zc(0), foot, zc(0));
  add_prism(zc(0), foot, zc(0.1), leg);
  add_prism_sync(zc(0.1), leg, zc(0.4), leg5);
  add_cone(zc(0.4), leg5, zc(0.45));
  add_texture(sh);
  // shift_last(-geom3::LEG0);
  for(int i=last->s; i<isize(hpc); i++) hpc[i] = cpush(0, -0.0125*S) * hpc[i];
  }

void make_head_only() {

  auto addpt = [] (int d, int u) {
    hpcpush(zpush(zc(eyepos) + 0.06 * SH * sin(u * degree)) * xspinpush0(d * degree, 0.05 * S * cos(u * degree)));
    };
  
  bshape(shPHeadOnly, shPHeadOnly.prio);
  last->flags |= POLY_TRIANGLES;
  for(int d=0; d<360; d+=30) 
  for(int u=-90; u<=90; u+=30) {
    addpt(d, u);
    addpt(d+30, u);
    addpt(d, u+30);
    addpt(d+30, u+30);
    addpt(d+30, u);
    addpt(d, u+30);
    }
  
  add_texture(shPHeadOnly);
  shift_last(-geom3::HEAD - revZ * 0.01 * SH); 
  }


void make_head_3d(hpcshape& sh) {
  auto head = get_shape(sh);
  vector<vector<array<ld, 2> >> pts(2);
  
  for(hyperpoint h: head) {
    array<ld, 2> p;
    p[0] = h[0] / h[3];
    p[1] = h[1] / h[3];
    pts[0].emplace_back(p);
    }
  
  array<ld, 2> zero = make_array<ld>(0,0);
  pts[1].emplace_back(zero);
  head.push_back(C0);
  
  bshape(sh, sh.prio);
  
  vector<int> indices = mapbox::earcut<int> (pts);
  
  last->flags |= POLY_TRIANGLES;
  for(int k=0; k<isize(indices); k+=3) {
    addtri(make_array(head[indices[k]], head[indices[k+1]], head[indices[k+2]]), 0);
    }
  
  add_texture(sh);
  shift_last(-geom3::HEAD);
  }

void make_paw_3d(hpcshape& sh, hpcshape& legsh) {
  auto foot = get_shape(sh);
  auto leg = get_shape(legsh);

  bshape(sh, PPR::MONSTER_BODY);
  shcenter = get_center(leg);
  add_cone(zc(0), foot, zc(0));
  add_prism(zc(0), foot, zc(0.1), leg);
  add_prism_sync(zc(0.1), leg, zc(0.4), leg);
  add_cone(zc(0.4), leg, zc(0.45));
  add_texture(sh);
  }

void make_abody_3d(hpcshape& sh, ld tail) {
  auto body = get_shape(sh);
  shcenter = get_center(body);

  vector<hyperpoint> notail;
  ld minx = 9;
  for(hyperpoint h: body) minx = min(minx, h[0]);
  for(hyperpoint h: body) if(h[0] >= minx + tail) notail.push_back(h);

  auto body8 = scaleshape(notail, 0.8);  
  
  bshape(sh, PPR::MONSTER_BODY);
  add_prism(zc(0.4), body8, zc(0.45), body);
  add_prism(zc(0.45), body, zc(0.5), notail);
  add_prism_sync(zc(0.6), body8, zc(0.5), notail);
  add_cone(zc(0.4), body8, zc(0.36));
  add_cone(zc(0.6), body8, zc(0.64));
  add_texture(sh);
  if(GDIM == 3 && WDIM == 2) shift_last(-geom3::ABODY);
  }

void make_ahead_3d(hpcshape& sh) {
  auto body = get_shape(sh);
  shcenter = get_center(body);
  auto body8 = scaleshape(body, 0.5);
  
  bshape(sh, PPR::MONSTER_BODY);
  add_prism_sync(zc(0.4), body8, zc(0.5), body);
  add_prism_sync(zc(0.6), body8, zc(0.5), body);
  add_cone(zc(0.4), body8, zc(0.36));
  add_cone(zc(0.6), body8, zc(0.64));
  add_texture(sh);
  }

void make_skeletal(hpcshape& sh, ld push = 0) {
  auto body = get_shape(sh);
  shcenter = get_center(body);
  
  bshape(sh, PPR::MONSTER_BODY);
  add_prism_sync(zc(0.48), body, zc(0.5), body);
  add_prism_sync(zc(0.52), body, zc(0.5), body);
  add_cone(zc(0.48), body, zc(0.47));
  add_cone(zc(0.52), body, zc(0.53));
  add_texture(sh);
  shift_last(-push);
  }

void make_revolution(hpcshape& sh, int mx = 180, ld push = 0) {
  auto body = get_shape(sh);
  bshape(sh, PPR::MONSTER_BODY);
  int step = (mx == 360 ? 24 : 10);
  for(int i=0; i<isize(body); i++) {
    hyperpoint h0 = body[i];
    hyperpoint h1 = body[(i+1) % isize(body)];
    for(int s=0; s<mx; s+=step) {
      hpcpush(cspin(1, 2, s * degree) * h0);
      hpcpush(cspin(1, 2, s * degree) * h1);
      hpcpush(cspin(1, 2, (s+step) * degree) * h0);
      hpcpush(cspin(1, 2, s * degree) * h1);
      hpcpush(cspin(1, 2, (s+step) * degree) * h0);
      hpcpush(cspin(1, 2, (s+step) * degree) * h1);
      }
    }
  last->flags |= POLY_TRIANGLES;
  add_texture(sh);
  shift_last(-push);
  }

void make_revolution_cut(hpcshape &sh, int each = 180, ld push = 0, ld width = 99) {
  auto body = get_shape(sh);
  body.resize(isize(body) / 2);
  ld fx = body[0][0];
  ld lx = body.back()[0];
  body.insert(body.begin(), hpxy(fx + (fx-lx) * 1e-3, 0));
  body.push_back(hpxy(lx + (lx-fx) * 1e-3, 0));
  int n = isize(body);
  
  auto gbody = body;
  
  int it = 0;
  
  vector<int> nextid(n);
  vector<int> lastid(n);
  vector<bool> stillin(n, true);
  for(int i=0; i<n; i++) nextid[i] = i+1;
  for(int i=0; i<n; i++) lastid[i] = i-1;
  nextid[n-1] = n-1; lastid[0] = 0;

  while(true) {
    it++;
    int cand = -1;
    ld cv = 0;
    for(int i=1; i<n-1; i++) if(stillin[i]) {
      if((gbody[i][0] < gbody[lastid[i]][0] && gbody[i][0] < gbody[nextid[i]][0]) || (gbody[i][0] > gbody[lastid[i]][0] && gbody[i][0] > gbody[nextid[i]][0]) || abs(gbody[i][1]) > width)
      if(abs(gbody[i][1]) > cv)
        cand = i, cv = abs(gbody[i][1]);
      }
    if(cand == -1) break;
    int i = cand;
    lastid[nextid[i]] = lastid[i];
    nextid[lastid[i]] = nextid[i];
    stillin[i] = false;
    }
  
  for(int i=n-1; i>=0; i--) if(!stillin[i] && !stillin[nextid[i]]) nextid[i] = nextid[nextid[i]];
  for(int i=0; i<n; i++) if(!stillin[i] && !stillin[lastid[i]]) lastid[i] = lastid[lastid[i]];

  for(int i=0; i<n; i++) {
    using namespace hyperpoint_vec;
    if(!stillin[i]) gbody[i] = normalize(gbody[lastid[i]] * (i - lastid[i]) + gbody[nextid[i]] * (nextid[i] - i));
    }

  bshape(sh, PPR::MONSTER_BODY);
  int step = 10;
  for(int i=0; i<n; i++) {
    for(int s=0; s<360; s+=step) {
      auto& tbody = (s % each ? gbody : body);
      auto& nbody = ((s+step) % each ? gbody : body);
      int i1 = (i+1) % isize(body);
      hyperpoint h0 = tbody[i];
      hyperpoint h1 = tbody[i1];
      hyperpoint hs0 = nbody[i];
      hyperpoint hs1 = nbody[i1];
      hpcpush(cspin(1, 2, s * degree) * h0);
      hpcpush(cspin(1, 2, s * degree) * h1);
      hpcpush(cspin(1, 2, (s+step) * degree) * hs0);
      hpcpush(cspin(1, 2, s * degree) * h1);
      hpcpush(cspin(1, 2, (s+step) * degree) * hs0);
      hpcpush(cspin(1, 2, (s+step) * degree) * hs1);
      }
    }
  last->flags |= POLY_TRIANGLES;
  add_texture(sh);
  shift_last(-push);
  }

void disable(hpcshape& sh) {
  sh.s = sh.e = 0;
  }

void clone_shape(hpcshape& sh, hpcshape& target) {
  target = sh;
  target.s = isize(hpc);
  for(int i=sh.s; i<sh.e; i++) hpc.push_back(hpc[i]);
  target.e = isize(hpc);
  }

void animate_bird(hpcshape& orig, hpcshape animated[30], ld body) {
  for(int i=0; i<30; i++) {
    auto& tgt = animated[i];
    clone_shape(orig, tgt);
    ld alpha = sin(12 * degree * i) * 30 * degree;
    for(int i=tgt.s; i<tgt.e; i++) {
      if(abs(hpc[i][1]) > body) {
        ld off = hpc[i][1] > 0 ? body : -body;
        hpc[i][2] += abs(hpc[i][1] - off) * sin(alpha);
        hpc[i][1] = off + (hpc[i][1] - off) * cos(alpha);
        hpc[i] = normalize(hpc[i]);
        }
      }
    }
  for(int i=0; i<30; i++) shift_shape(animated[i], geom3::BIRD);
  shift_shape(orig, geom3::BIRD);
  }

void slimetriangle(hyperpoint a, hyperpoint b, hyperpoint c, ld rad, int lev) {
  dynamicval<int> d(vid.texture_step, 8);
  texture_order([&] (ld x, ld y) {
    using namespace hyperpoint_vec;
    ld z = 1-x-y;
    ld r = scalefactor * hcrossf7 * (0 + pow(max(x,max(y,z)), .3) * 0.8);
    hyperpoint h = rspintox(a*x+b*y+c*z) * xpush0(r);
    hpcpush(h);
    });  
  }

void balltriangle(hyperpoint a, hyperpoint b, hyperpoint c, ld rad, int lev) {
  if(lev == 0) {
    hpcpush(a);
    hpcpush(b);
    hpcpush(c);
    }
  else {
    hyperpoint cx = rspintox(mid(a,b)) * xpush0(rad);
    hyperpoint ax = rspintox(mid(b,c)) * xpush0(rad);
    hyperpoint bx = rspintox(mid(c,a)) * xpush0(rad);
    balltriangle(ax, bx, cx, rad, lev-1);
    balltriangle(ax, bx, c , rad, lev-1);
    balltriangle(ax, b , cx, rad, lev-1);
    balltriangle(a , bx, cx, rad, lev-1);
    }
  }

void make_ball(hpcshape& sh, ld rad, int lev) {
  bshape(sh, sh.prio);
  sh.flags |= POLY_TRIANGLES;
  hyperpoint tip = xpush0(rad);
  hyperpoint atip = xpush0(-rad);
  ld z = 63.43 * degree;
  for(int i=0; i<5; i++) {
    hyperpoint a = cspin(1, 2, (72 * i   ) * degree) * spin(z) * tip;
    hyperpoint b = cspin(1, 2, (72 * i-72) * degree) * spin(z) * tip;
    hyperpoint c = cspin(1, 2, (72 * i+36) * degree) * spin(M_PI-z) * tip;
    hyperpoint d = cspin(1, 2, (72 * i-36) * degree) * spin(M_PI-z) * tip;
    balltriangle(tip, a, b, rad, lev);
    balltriangle(a, b, c, rad, lev);
    balltriangle(b, c, d, rad, lev);
    balltriangle(c, d, atip, rad, lev);
    }
  add_texture(sh);
  }

hyperpoint psmin(hyperpoint H) {
  hyperpoint res;
  res[2] = asin_auto(H[2]);
  ld cs = pow(cos_auto(res[2]), 2);
  ld r = sqrt(cs+H[0]*H[0]+H[1]*H[1]);
  res[0] = H[0] / r;
  res[1] = H[1] / r;
  return res;
  }

void adjust_eye(hpcshape& eye, hpcshape head, ld shift_eye, ld shift_head, int q, ld zoom=1) {
  using namespace hyperpoint_vec;
  hyperpoint center = Hypc;
  for(int i=eye.s; i<eye.e; i++) if(q == 1 || hpc[i][1] > 0) center += hpc[i];
  center = normalize(center);
  // center /= (eye.e - eye.s);
  ld rad = 0;
  for(int i=eye.s; i<eye.e; i++) if(q == 1 || hpc[i][1] > 0) rad += hdist(center, hpc[i]);
  rad /= (eye.e - eye.s);
  
  hyperpoint pscenter = psmin(center);
  
  ld pos = 0;
  int qty = 0, qtyall = 0;
  
  vector<hyperpoint> pss;
  
  for(int i=head.s; i<head.e; i++) pss.push_back(psmin(zpush(shift_head) * hpc[i]));
  
  ld zmid = 0;
  for(hyperpoint& h: pss) zmid += h[2];
  zmid /= isize(pss);
  
  ld mindist = 1e9;
  for(int i=0; i<isize(pss); i+=3) if(pss[i][2] < zmid || WDIM == 3) {
    ld d = sqhypot_d(2, pss[i]-pscenter) + sqhypot_d(2, pss[i+1]-pscenter) + sqhypot_d(2, pss[i+2]-pscenter);
    if(d < mindist) mindist = d, pos = min(min(pss[i][2], pss[i+1][2]), pss[i+2][2]), qty++;
    qtyall++;
    }
  
  if(&eye == &shSkullEyes) pos = zc(eyepos) - 0.06 * SH * 0.05;
  
  make_ball(eye, rad, 0);
  transmatrix T = zpush(-shift_eye) * rgpushxto0(center) * zpush(pos); 
  for(int i=eye.s; i<isize(hpc); i++) hpc[i] = T * hpc[i];
  int s = isize(hpc);
  if(&eye == &shSkullEyes) 
    for(int i=eye.s; i<s; i++) hpc[i] = xpush(0.07 * scalefactor) * hpc[i];
  if(q == 2)
    for(int i=eye.s; i<s; i++) hpcpush(MirrorY * hpc[i]);

  finishshape();
  // eye.prio = PPR::SUPERLINE;
  }

void shift_last_straight(ld z) {
  for(int i=last->s; i<isize(hpc); i++) hpc[i] = zpush(z) * hpc[i];
  }

void queueball(const transmatrix& V, ld rad, color_t col, eItem what) {
  if(what == itOrbSpeed) {
    transmatrix V1 = V * cspin(1, 2, M_PI/2);
    ld tt = ptick(100);
    for(int t=0; t<5; t++) {
      for(int a=-50; a<50; a++)
        curvepoint(V1 * cspin(0, 2, a * M_PI/100.) * cspin(0, 1, t * 72 * degree + tt + a*2*M_PI/50.) * xpush0(rad));
      queuecurve(col, 0, PPR::LINE);
      }
    return;
    }
  ld z = 63.43 * degree;
  transmatrix V1 = V * cspin(0, 2, M_PI/2);
  if(what == itOrbShield) V1 = V * cspin(0, 1, ptick(500));
  if(what == itOrbFlash) V1 = V * cspin(0, 1, ptick(1500));
  if(what == itOrbShield) V1 = V * cspin(1, 2, ptick(1000));
  if(what == itOrbFlash) V1 = V * cspin(1, 2, ptick(750));
  if(what == itDiamond) V1 = V * cspin(1, 2, ptick(200));
  if(what == itRuby) V1 = V * cspin(1, 2, ptick(300));
  auto line = [&] (transmatrix A, transmatrix B) {
    hyperpoint h0 = A * xpush0(1);
    hyperpoint h1 = B * xpush0(1);
    using namespace hyperpoint_vec;
    for(int i=0; i<=8; i++)
      curvepoint(V1 * rspintox(normalize(h0*(8-i) + h1*i)) * xpush0(rad));
    queuecurve(col, 0, PPR::LINE);
    };
  for(int i=0; i<5; i++) {
    auto a = cspin(1, 2, (72 * i   ) * degree) * spin(z);
    auto b = cspin(1, 2, (72 * i-72) * degree) * spin(z);
    auto c = cspin(1, 2, (72 * i+36) * degree) * spin(M_PI-z);
    auto d = cspin(1, 2, (72 * i-36) * degree) * spin(M_PI-z);
    line(Id, a);
    line(a, b);
    line(a, c);
    line(a, d);
    line(d, c);
    line(c, spin(M_PI));
    }
  }

void make_shadow(hpcshape& sh) {
  sh.shs = isize(hpc);
  for(int i=sh.s; i < sh.e; i++) hpcpush(orthogonal_move(hpc[i], geom3::FLOOR - geom3::human_height / 100));
  sh.she = isize(hpc);
  }

void make_3d_models() {
  if(DIM == 2) return;
  eyepos = WDIM == 2 ? 0.875 : 0.925;
  DEBBI(DF_POLY, ("make_3d_models"));
  shcenter = C0;
  
  if(floor_textures) {
    auto& utt = models_texture;
    utt.tvertices.clear();
    utt.texture_id = floor_textures->renderedTexture;
    }
  
  if(WDIM == 2) {
    DEBB(DF_POLY, ("shadows"));
    for(hpcshape* sh: {&shBatWings, &shBugBody, &shBullBody, &shButterflyWing, &shCatBody, &shDogBody, &shDogTorso,
      &shEagle, &shFemaleBody, &shFlailMissile, &shGadflyWing, &shGargoyleWings, &shHawk, &shJiangShi, &shKnife,
      &shPBody, &shPHead, &shRaiderBody, &shReptileBody, &shSkeletonBody, &shTongue, &shTrapArrow, &shTrylobite,
      &shWaterElemental, &shWolfBody, &shYeti})
      make_shadow(*sh);
    
    for(int i=0; i<8; i++) make_shadow(shAsteroid[i]);
    }
    
  DEBB(DF_POLY, ("humanoids"));
  make_humanoid_3d(shPBody);
  make_humanoid_3d(shYeti);
  make_humanoid_3d(shFemaleBody);
  make_humanoid_3d(shRaiderBody);
  make_humanoid_3d(shSkeletonBody);
  make_humanoid_3d(shFatBody);
  make_humanoid_3d(shWaterElemental);
  make_humanoid_3d(shJiangShi);
  
  // shFatBody = shPBody;
  // shFemaleBody = shPBody;
  // shRaiderBody = shPBody;
  // shJiangShi = shPBody;

  DEBB(DF_POLY, ("heads"));
  make_head_3d(shFemaleHair);
  make_head_3d(shPHead);
  make_head_3d(shTurban1);
  make_head_3d(shTurban2);
  make_head_3d(shAztecHead);
  make_head_3d(shAztecCap);
  make_head_3d(shVikingHelmet);
  make_head_3d(shRaiderHelmet);
  make_head_3d(shWestHat1);
  make_head_3d(shWestHat2);
  make_head_3d(shWitchHair);
  make_head_3d(shBeautyHair);
  make_head_3d(shFlowerHair);
  make_head_3d(shGolemhead);
  make_head_3d(shPirateHood);
  make_head_3d(shEyepatch);
  make_head_3d(shSkull);
  make_head_3d(shRatHead);
  make_head_3d(shDemon);
  make_head_3d(shGoatHead);
  make_head_3d(shRatCape1);
  make_head_3d(shJiangShiCap1);
  make_head_3d(shJiangShiCap2);
  make_head_3d(shTerraHead);
  
  DEBB(DF_POLY, ("armors"));
  make_armor_3d(shKnightArmor);
  make_armor_3d(shKnightCloak, 2);
  make_armor_3d(shPrinceDress);
  make_armor_3d(shPrincessDress, 2);
  make_armor_3d(shTerraArmor1);
  make_armor_3d(shTerraArmor2);
  make_armor_3d(shTerraArmor3); 
  make_armor_3d(shSuspenders); 
  make_armor_3d(shJiangShiDress); 
  make_armor_3d(shFemaleDress); 
  make_armor_3d(shWightCloak, 2); 
  make_armor_3d(shRaiderArmor); 
  make_armor_3d(shRaiderShirt); 
  make_armor_3d(shArmor); 
  make_armor_3d(shRatCape2, 2); 
  
  make_armor_3d(shHood, 2);
  
  DEBB(DF_POLY, ("feet and paws"));
  make_foot_3d(shHumanFoot);
  make_foot_3d(shYetiFoot);
  make_skeletal(shSkeletalFoot, WDIM == 2 ? zc(0.5) + geom3::human_height/40 - geom3::FLOOR : 0);
  
  make_paw_3d(shWolfFrontPaw, shWolfFrontLeg);
  make_paw_3d(shWolfRearPaw, shWolfRearLeg);
  make_paw_3d(shDogFrontPaw, shDogFrontLeg);
  make_paw_3d(shDogRearPaw, shDogRearLeg);  
  
  DEBB(DF_POLY, ("revolution"));
  // make_abody_3d(shWolfBody, 0.01);
  // make_ahead_3d(shWolfHead);
  // make_ahead_3d(shFamiliarHead);
  ld g = WDIM == 2 ? geom3::ABODY - zc(0.4) : 0;
  
  make_revolution_cut(shWolfBody, 30, g, 0.01*S);
  make_revolution_cut(shWolfHead, 180, geom3::AHEAD - geom3::ABODY +g);
  make_revolution_cut(shFamiliarHead, 30, geom3::AHEAD - geom3::ABODY +g);

  // make_abody_3d(shDogTorso, 0.01);
  make_revolution_cut(shDogTorso, 30, +g);
  make_revolution_cut(shDogHead, 180, geom3::AHEAD - geom3::ABODY +g);
  // make_ahead_3d(shDogHead);

  // make_abody_3d(shCatBody, 0.05);
  // make_ahead_3d(shCatHead);
  make_revolution_cut(shCatBody, 30, +g);
  make_revolution_cut(shCatHead, 180, geom3::AHEAD - geom3::ABODY +g, 0.055 * scalefactor);

  make_paw_3d(shReptileFrontFoot, shReptileFrontLeg);
  make_paw_3d(shReptileRearFoot, shReptileRearLeg);  
  make_abody_3d(shReptileBody, -1);
  // make_ahead_3d(shReptileHead);
  make_revolution_cut(shReptileHead, 180, geom3::AHEAD - geom3::ABODY+g);

  make_paw_3d(shBullFrontHoof, shBullFrontHoof);
  make_paw_3d(shBullRearHoof, shBullRearHoof);
  // make_abody_3d(shBullBody, 0.05);
  // make_ahead_3d(shBullHead);
  // make_ahead_3d(shBullHorn);
  make_revolution_cut(shBullBody, 180, +g);
  make_revolution_cut(shBullHead, 60, geom3::AHEAD - geom3::ABODY +g);
  shift_shape(shBullHorn, g-(geom3::AHEAD - geom3::ABODY));
  // make_revolution_cut(shBullHorn, 180, geom3::AHEAD - geom3::ABODY);
  
  make_paw_3d(shTrylobiteFrontClaw, shTrylobiteFrontLeg);
  make_paw_3d(shTrylobiteRearClaw, shTrylobiteRearLeg);
  make_abody_3d(shTrylobiteBody, 0);
  // make_ahead_3d(shTrylobiteHead);
  make_revolution_cut(shTrylobiteHead, 180, geom3::AHEAD - geom3::ABODY +g);
  
  make_revolution_cut(shShark, 180, WDIM == 2 ? -geom3::FLOOR : 0);

  make_revolution_cut(shGhost, 60, geom3::GHOST + g);

  make_revolution_cut(shEagle, 180, -geom3::BIRD, 0.05*S);
  make_revolution_cut(shHawk, 180, -geom3::BIRD, 0.05*S);

  make_revolution_cut(shTinyBird, 180, 0, 0.025 * S);
  make_revolution_cut(shTinyShark, 90);
  make_revolution_cut(shMiniGhost, 60);

  make_revolution_cut(shGargoyleWings, 180, 0, 0.05*S);
  make_revolution_cut(shGargoyleBody, 180, 0, 0.05*S);
  make_revolution_cut(shGadflyWing, 180, -geom3::BIRD, 0.05*S);
  make_revolution_cut(shBatWings, 180, -geom3::BIRD, 0.05*S);
  make_revolution_cut(shBatBody, 180, -geom3::BIRD, 0.05*S);

  make_revolution_cut(shJelly, 60);
  make_revolution(shFoxTail1);
  make_revolution(shFoxTail2);
  make_revolution(shGadflyBody, 180, -geom3::BIRD);
  for(int i=0; i<8; i++)
    make_revolution(shAsteroid[i], 360);
  
  make_revolution_cut(shBugLeg, 60);

  make_revolution(shBugArmor, 180, geom3::ABODY);
  make_revolution_cut(shBugAntenna, 90, geom3::ABODY);
  
  make_revolution_cut(shButterflyBody, 180, -geom3::BIRD);
  
  animate_bird(shEagle, shAnimatedEagle, 0.05*S);
  animate_bird(shTinyBird, shAnimatedTinyEagle, 0.05*S/2);

  disable(shWolfRearLeg);
  disable(shWolfFrontLeg);
  disable(shDogRearLeg);
  disable(shDogFrontLeg);
  disable(shReptileFrontLeg);
  disable(shReptileRearLeg);
  disable(shTrylobiteFrontLeg);
  disable(shTrylobiteRearLeg);
  disable(shPFace);
  disable(shJiangShi);
  
  make_revolution_cut(shDragonSegment, 60, g);
  make_revolution_cut(shDragonHead, 60, g);
  make_revolution_cut(shDragonTail, 60, g);
  make_revolution_cut(shWormSegment, 60, g);
  make_revolution_cut(shSmallWormSegment, 60, g);
  make_revolution_cut(shWormHead, 60, g);
  make_revolution_cut(shWormTail, 60, g);
  make_revolution_cut(shSmallWormTail, 60, g);
  make_revolution_cut(shTentHead, 60, g);
  make_revolution_cut(shKrakenHead, 60, -geom3::FLOOR);
  make_revolution_cut(shSeaTentacle, 60, -geom3::FLOOR);
  make_revolution_cut(shDragonLegs, 60, g);
  make_revolution_cut(shDragonWings, 60, g);
  disable(shDragonNostril);

  make_head_only();
  
  DEBB(DF_POLY, ("balls"));
  make_ball(shDisk, orbsize*.2, 2);
  make_ball(shHeptaMarker, zhexf*.2, 1);
  make_ball(shSnowball, zhexf*.1, 0);
  
  if(WDIM == 2) {
    for(int i=0; i<3; i++)
      shift_shape(shHalfFloor[i], geom3::lev_to_factor(geom3::human_height * .01));
    }

  shift_shape(shBoatOuter, geom3::FLOOR);
  shift_shape(shBoatInner, (geom3::FLOOR+geom3::LAKE)/2);
  
  for(int i=0; i<14; i++)
    shift_shape(shTriheptaSpecial[i], geom3::FLOOR);

  shift_shape(shBigCarpet1, geom3::FLOOR - geom3::human_height * 1/40);
  shift_shape(shBigCarpet2, geom3::FLOOR - geom3::human_height * 2/40);
  shift_shape(shBigCarpet3, geom3::FLOOR - geom3::human_height * 3/40);
  for(int a=0; a<5; a++) for(int b=0; b<4; b++)
    shift_shape(shReptile[a][b], geom3::FLOOR - geom3::human_height * min(b, 2) / 40);
    
  shift_shape(shMineMark[0], geom3::FLOOR - geom3::human_height * 1/40);
  shift_shape(shMineMark[1], geom3::FLOOR - geom3::human_height * 1/40);

  for(int a=0; a<5; a++) shift_shape(shZebra[a], geom3::FLOOR);
  
  for(int t=0; t<13; t++) for(int u=0; u<6; u++)
    shift_shape(shTortoise[t][u], geom3::FLOOR - geom3::human_height * (t+1) / 120);

  make_revolution_cut(shStatue, 60);
  
  shift_shape(shThorns, geom3::FLOOR - geom3::human_height * 1/40);
  shift_shape(shRose, geom3::FLOOR - geom3::human_height * 1/20);

  DEBB(DF_POLY, ("slime"));
  bshape(shSlime, PPR::MONSTER_BODY);
  hyperpoint tip = xpush0(1);
  hyperpoint atip = xpush0(-1);
  ld z = 63.43 * degree;
  for(int i=0; i<5; i++) {
    auto a = cspin(1, 2, (72 * i   ) * degree) * spin(z) * xpush0(1);
    auto b = cspin(1, 2, (72 * i-72) * degree) * spin(z) * xpush0(1);
    auto c = cspin(1, 2, (72 * i+36) * degree) * spin(M_PI-z) * xpush0(1);
    auto d = cspin(1, 2, (72 * i-36) * degree) * spin(M_PI-z) * xpush0(1);
    slimetriangle(tip, a, b, 1, 0);
    slimetriangle(a, b, c, 1, 0);
    slimetriangle(b, c, d, 1, 0);
    slimetriangle(c, d, atip, 1, 0);    
    }
  last->flags |= POLY_TRIANGLES;
  add_texture(*last);
  if(WDIM == 2) shift_last_straight(geom3::FLOOR);
  finishshape();
  shJelly = shSlime;
  
  shift_shape(shMagicSword, geom3::ABODY);
  shift_shape(shMagicShovel, geom3::ABODY);
  
  DEBB(DF_POLY, ("eyes"));
  adjust_eye(shSlimeEyes, shSlime, geom3::FLATEYE, 0, 2, 2);
  adjust_eye(shGhostEyes, shGhost, geom3::GHOST, geom3::GHOST, 2, WDIM == 2 ? 2 : 4);
  adjust_eye(shMiniEyes, shMiniGhost, geom3::GHOST, geom3::GHOST, 2, 2);
  adjust_eye(shWormEyes, shWormHead, 0, 0, 2, 4);
  adjust_eye(shDragonEyes, shDragonHead, 0, 0, 2, 4);
  
  adjust_eye(shKrakenEye, shKrakenHead, 0, 0, 1);
  adjust_eye(shKrakenEye2, shKrakenEye, 0, 0, 1, 2);
  
  adjust_eye(shWolf1, shDogHead, geom3::AHEAD, geom3::AHEAD, 1); 
  adjust_eye(shWolf2, shDogHead, geom3::AHEAD, geom3::AHEAD, 1); 
  adjust_eye(shWolf3, shDogHead, geom3::AHEAD, geom3::AHEAD, 1);
  adjust_eye(shFamiliarEye, shWolfHead, geom3::AHEAD, geom3::AHEAD, 1);
  
  adjust_eye(shWolfEyes, shWolfHead, geom3::AHEAD, geom3::AHEAD, 1);

  adjust_eye(shReptileEye, shReptileHead, geom3::AHEAD, geom3::AHEAD, 1);
  adjust_eye(shGadflyEye, shGadflyBody, -geom3::BIRD, -geom3::BIRD, 1);
  
  adjust_eye(shSkullEyes, shPHeadOnly, geom3::HEAD1, geom3::HEAD, 2, 2);
  shSkullEyes.tinf = NULL;

  shift_shape(shRatTail, zc(0.5) - geom3::LEG);
  for(int i=shRatTail.s; i<shRatTail.e; i++) hpc[i] = xpush(-scalefactor * 0.1) * hpc[i];
  }

#undef S
#undef SH
#undef revZ
#endif

}
