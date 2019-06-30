<?xml version="1.0" encoding="UTF-8"?>
<tileset name="walker" tilewidth="16" tileheight="16" tilecount="4" columns="4">
 <image source="walker.png" width="64" height="16"/>
 <tile id="0" type="ACTOR_TEMPLATE">
  <properties>
   <property name="ACTOR_NAME" value="WALKER"/>
   <property name="ON_ALWAYS" value="Movement"/>
  </properties>
 </tile>
 <tile id="1" type="ACTOR_ANIMATION">
  <properties>
   <property name="ACTOR_NAME" value="WALKER"/>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="DIRECTION" value="DOWN"/>
  </properties>
 </tile>
</tileset>
