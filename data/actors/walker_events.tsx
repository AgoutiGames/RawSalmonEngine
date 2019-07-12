<?xml version="1.0" encoding="UTF-8"?>
<tileset name="walker_events" tilewidth="60" tileheight="60" tilecount="100" columns="10">
 <image source="../tilesets/symbol_numbers.png" width="600" height="600"/>
 <tile id="0" type="AeAccelerate">
  <properties>
   <property name="NAME" value="MoveLeft"/>
   <property name="XFACTOR" type="float" value="-500"/>
   <property name="XMAX_SPEED" type="float" value="-500"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="1" type="AeAccelerate">
  <properties>
   <property name="NAME" value="MoveRight"/>
   <property name="XFACTOR" type="float" value="500"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="2" type="AeAccelerate">
  <properties>
   <property name="NAME" value="MoveUp"/>
   <property name="XFACTOR" type="float" value="0"/>
   <property name="YFACTOR" type="float" value="-500"/>
   <property name="YMAX_SPEED" type="float" value="-500"/>
  </properties>
 </tile>
 <tile id="3" type="AeAccelerate">
  <properties>
   <property name="NAME" value="MoveDown"/>
   <property name="XFACTOR" type="float" value="0"/>
   <property name="YFACTOR" type="float" value="500"/>
  </properties>
 </tile>
 <tile id="4" type="AeMove">
  <properties>
   <property name="NAME" value="Move"/>
  </properties>
 </tile>
 <tile id="5" type="AeDecelerate">
  <properties>
   <property name="NAME" value="Decelerate"/>
   <property name="XFACTOR" type="float" value="200"/>
   <property name="YFACTOR" type="float" value="200"/>
  </properties>
 </tile>
 <tile id="6" type="AeMulti">
  <properties>
   <property name="1" value="Decelerate"/>
   <property name="2" value="Move"/>
   <property name="NAME" value="Movement"/>
  </properties>
 </tile>
 <tile id="9" type="AeMoveStatic">
  <properties>
   <property name="NAME" value="PositionUpLeft"/>
   <property name="X_POS" type="float" value="0.3"/>
   <property name="Y_POS" type="float" value="0.3"/>
  </properties>
 </tile>
</tileset>
