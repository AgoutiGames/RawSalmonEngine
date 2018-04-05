<?xml version="1.0" encoding="UTF-8"?>
<tileset name="block_animation" tilewidth="128" tileheight="128" tilecount="12" columns="4">
 <image source="../actors/block_animation.png" width="512" height="384"/>
 <tile id="0" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="DEF_BLOCK"/>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="DIRECTION" value="UP"/>
  </properties>
  <objectgroup draworder="index"/>
  <animation>
   <frame tileid="0" duration="100"/>
   <frame tileid="1" duration="100"/>
   <frame tileid="2" duration="100"/>
   <frame tileid="3" duration="100"/>
  </animation>
 </tile>
 <tile id="1" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="DEF_BLOCK"/>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="DIRECTION" value="RIGHT"/>
  </properties>
  <animation>
   <frame tileid="1" duration="100"/>
   <frame tileid="2" duration="100"/>
   <frame tileid="3" duration="100"/>
   <frame tileid="0" duration="100"/>
  </animation>
 </tile>
 <tile id="2" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="DEF_BLOCK"/>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="DIRECTION" value="DOWN"/>
  </properties>
  <animation>
   <frame tileid="2" duration="100"/>
   <frame tileid="3" duration="100"/>
   <frame tileid="0" duration="100"/>
   <frame tileid="1" duration="100"/>
  </animation>
 </tile>
 <tile id="3" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="DEF_BLOCK"/>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="DIRECTION" value="LEFT"/>
  </properties>
  <animation>
   <frame tileid="3" duration="100"/>
   <frame tileid="0" duration="100"/>
   <frame tileid="1" duration="100"/>
   <frame tileid="2" duration="100"/>
  </animation>
 </tile>
 <tile id="4" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="DEF_BLOCK"/>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="DIRECTION" value="UP"/>
  </properties>
  <animation>
   <frame tileid="4" duration="100"/>
   <frame tileid="5" duration="100"/>
   <frame tileid="6" duration="100"/>
   <frame tileid="7" duration="100"/>
  </animation>
 </tile>
 <tile id="5" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="DEF_BLOCK"/>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="DIRECTION" value="RIGHT"/>
  </properties>
  <objectgroup draworder="index"/>
  <animation>
   <frame tileid="5" duration="100"/>
   <frame tileid="6" duration="100"/>
   <frame tileid="7" duration="100"/>
   <frame tileid="4" duration="100"/>
  </animation>
 </tile>
 <tile id="6" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="DEF_BLOCK"/>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="DIRECTION" value="DOWN"/>
  </properties>
  <animation>
   <frame tileid="6" duration="100"/>
   <frame tileid="5" duration="100"/>
   <frame tileid="4" duration="100"/>
   <frame tileid="7" duration="100"/>
  </animation>
 </tile>
 <tile id="7" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="DEF_BLOCK"/>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="DIRECTION" value="LEFT"/>
  </properties>
  <animation>
   <frame tileid="7" duration="100"/>
   <frame tileid="6" duration="100"/>
   <frame tileid="5" duration="100"/>
   <frame tileid="4" duration="100"/>
  </animation>
 </tile>
</tileset>
