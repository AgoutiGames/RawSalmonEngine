<?xml version="1.0" encoding="UTF-8"?>
<tileset name="coin_handle" tilewidth="16" tileheight="16" tilecount="1" columns="1">
 <image source="coin_handle.png" width="16" height="16"/>
 <tile id="0" type="ACTOR_TEMPLATE">
  <properties>
   <property name="ACTOR_NAME" value="COIN"/>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="DIRECTION" value="NONE"/>
   <property name="ON_COLLISION" value="CoinPickupSafe"/>
  </properties>
  <objectgroup draworder="index">
   <object id="1" x="1" y="1" width="14" height="14"/>
  </objectgroup>
 </tile>
</tileset>
