<?xml version="1.0" encoding="UTF-8"?>
<tileset name="actor_animation" tilewidth="32" tileheight="32" tilecount="8" columns="4">
 <image source="new_anim.png" width="128" height="64"/>
 <tile id="0" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="BLOCKY"/>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="DIRECTION" value="RIGHT"/>
  </properties>
  <objectgroup draworder="index"/>
  <animation>
   <frame tileid="0" duration="100"/>
   <frame tileid="1" duration="100"/>
   <frame tileid="2" duration="100"/>
   <frame tileid="3" duration="100"/>
  </animation>
 </tile>
 <tile id="1">
  <objectgroup draworder="index"/>
 </tile>
 <tile id="2">
  <objectgroup draworder="index"/>
 </tile>
 <tile id="3" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="BLOCKY"/>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="DIRECTION" value="LEFT"/>
  </properties>
  <objectgroup draworder="index"/>
  <animation>
   <frame tileid="0" duration="100"/>
   <frame tileid="3" duration="100"/>
   <frame tileid="2" duration="100"/>
   <frame tileid="1" duration="100"/>
  </animation>
 </tile>
 <tile id="4" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="BLOCKY"/>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="DIRECTION" value="RIGHT"/>
  </properties>
  <animation>
   <frame tileid="4" duration="100"/>
   <frame tileid="5" duration="100"/>
   <frame tileid="6" duration="100"/>
   <frame tileid="7" duration="100"/>
   <frame tileid="6" duration="100"/>
   <frame tileid="5" duration="100"/>
  </animation>
 </tile>
 <tile id="7" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="BLOCKY"/>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="DIRECTION" value="LEFT"/>
  </properties>
  <animation>
   <frame tileid="4" duration="100"/>
   <frame tileid="5" duration="100"/>
   <frame tileid="6" duration="100"/>
   <frame tileid="7" duration="100"/>
   <frame tileid="6" duration="100"/>
   <frame tileid="5" duration="100"/>
  </animation>
 </tile>
</tileset>
