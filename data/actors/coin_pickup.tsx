<?xml version="1.0" encoding="UTF-8"?>
<tileset name="coin_pickup" tilewidth="16" tileheight="16" tilecount="5" columns="1">
 <image source="coin.png" width="16" height="80"/>
 <tile id="3" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="COIN"/>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="DIRECTION" value="DOWN"/>
  </properties>
  <animation>
   <frame tileid="4" duration="100"/>
   <frame tileid="3" duration="100"/>
   <frame tileid="2" duration="100"/>
   <frame tileid="1" duration="100"/>
   <frame tileid="0" duration="100"/>
  </animation>
 </tile>
 <tile id="4" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="COIN"/>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="DIRECTION" value="DOWN"/>
  </properties>
 </tile>
</tileset>
