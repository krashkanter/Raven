#include "../common/example.h"
#include <stddef.h>
#include <math.h>

static RLiquidGlass look;
static Rid targets[3], value_labels[24];
static float corner_radius = 38, opacity = 1;
typedef struct Control { const char* name; size_t offset; float min, max; } Control;
#define F(field, label, lo, hi) {label, offsetof(RLiquidGlass, field), lo, hi}
static const Control controls[] = {
 F(frost,"Frost",0,1), F(refraction,"Refraction",0,96), F(bevel,"Bevel",0,96),
 F(saturation,"Saturation",0,4), F(glow,"Glow",-1,1), F(shadow,"Shadow softness",0,96),
 F(dispersion,"Chromatic dispersion",0,1), F(reflection,"Reflection",0,1), F(light_angle,"Light direction",0,360),
 F(tint[0],"Tint / red",0,1), F(tint[1],"Tint / green",0,1), F(tint[2],"Tint / blue",0,1), F(tint[3],"Tint / opacity",0,1),
 F(rim[0],"Rim / red",0,1), F(rim[1],"Rim / green",0,1), F(rim[2],"Rim / blue",0,1), F(rim[3],"Rim / intensity",0,1),
 F(shadow_color[0],"Shadow / red",0,1), F(shadow_color[1],"Shadow / green",0,1), F(shadow_color[2],"Shadow / blue",0,1), F(shadow_color[3],"Shadow / opacity",0,1)
};
static void apply(void) {
 int i;
 for(i=0;i<3;i++) { Rwidget_set_liquid_glass(targets[i], &look); Rwidget_set_opacity(targets[i], opacity); }
 Rborder_set_corner_radius(targets[0],corner_radius,corner_radius,corner_radius,corner_radius);
}
static void changed(Rid slider, float value, void* data) {
 size_t index=(size_t)data; char text[100]; (void)slider;
 if(index < sizeof(controls)/sizeof(controls[0])) {
  *(float*)((char*)&look+controls[index].offset)=value;
  snprintf(text,sizeof(text),"%s  %.2f",controls[index].name,(double)value);
 } else if(index==21) { corner_radius=value; snprintf(text,sizeof(text),"Corner radius  %.1f",(double)value); }
 else { opacity=value; snprintf(text,sizeof(text),"Surface opacity  %.2f",(double)value); }
 Rlabel_set_text(value_labels[index],text); apply();
}
static void toggled(Rid widget,int checked,void* data) {
 int index=(int)(size_t)data; (void)widget;
 if(index==0) look.interactive=checked;
 if(index==1) look.reduced_transparency=checked;
 if(index==2) look.high_contrast=checked;
 if(index==3) { Rtheme_apply(checked?R_THEME_LIGHT:R_THEME_DARK); Rtheme_set_color(R_COLOR_TEXT,1,1,1,1); }
 if(index==4) Rset_animations_enabled(!checked);
 apply();
}
static void backdrop(Rid widget,RPainter* painter,float width,float height,void* data) {
 int x,y; (void)widget;(void)data;
 for(y=0;y<(int)height;y+=8) for(x=0;x<(int)width;x+=8) {
  float u=x/width,v=y/height;
  float a=expf(-8*((u-.25f)*(u-.25f)+(v-.4f)*(v-.4f)));
  float b=expf(-9*((u-.8f)*(u-.8f)+(v-.7f)*(v-.7f)));
  Rdraw_rect(painter,(float)x,(float)y,8,8,0,.08f+.15f*a+.70f*b,.10f+.28f*a+.16f*b,.21f+.68f*a+.2f*b,1);
 }
 for(x=0;x<12;x++) Rdraw_line(painter, (float)x*100-200, height, (float)x*100+200, 0, 2, .8f,.9f,1,.25f);
 Rdraw_circle(painter,width*.74f,height*.4f,85,1,.45f,.2f,1);
 Rdraw_circle(painter,width*.28f,height*.72f,110,.6f,.2f,.9f,1);
}
int main(void) {
 Rid window,box; size_t i;
 const char* toggles[]={"Interactive highlights","Reduce transparency","Increase contrast","Light theme","Reduce motion"};
 if(!example_init("Liquid Glass Lab")) return 1;
 Rtheme_apply(R_THEME_DARK); Rtheme_set_color(R_COLOR_TEXT,1,1,1,1);
 window=Rwindow_create_from_xml_file(RAVEN_EXAMPLE_DIR "/demo.xml");
 if(!Rwindow_is_valid(window)) { Rshutdown();return 2; }
 targets[0]=Rwidget_find(window,"hero");targets[1]=Rwidget_find(window,"pill");targets[2]=Rwidget_find(window,"action");
 Rliquid_glass_defaults(&look); apply();
 Rcustom_set_paint_callback(Rwidget_find(window,"backdrop"),backdrop,NULL);
 box=Rwidget_find(window,"controls");
 for(i=0;i<5;i++) {
  Rid c=Rwidget_add_child(box,Rcheckbox_create(toggles[i]));
  Rcheckbox_set_checked(c,i==0);
  Rcheckbox_set_changed_callback(c,toggled,(void*)i);
 }
 for(i=0;i<23;i++) {
  Rid slider=Rwidget_create(R_WIDGET_SLIDER);
  float value=i<21?*(float*)((char*)&look+controls[i].offset):(i==21?corner_radius:opacity);
  value_labels[i]=Rwidget_add_child(box,Rlabel_create(""));
  Rwidget_add_child(box,slider);
  Rslider_set_range(slider,i<21?controls[i].min:0,i<21?controls[i].max:(i==21?100:1));
  Rslider_set_value(slider,value);
  changed(slider,value,(void*)i);
  Rslider_set_changed_callback(slider,changed,(void*)i);
 }
 if(!example_automated(window)) while(Rwait_frame(-1)) {}
 Rshutdown();return 0;
}

