textures//wsw_flareshalos/flare_neontube_white1
{
	qer_editorimage textures//wsw_industrial1/neon_flare_white1.png
	qer_trans 0.25
	cull back
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nonsolid
	deformVertexes autosprite2
	forceoverlays
	nopicmip
	softParticle

	{
		detail
		clampmap textures//wsw_industrial1/neon_flare_white1.png
		blendFunc add
	}
}

textures/wsw_flareshalos/glow_rec_orange
{
	qer_editorimage textures/wsw_flareshalos/glow_rec_orange.png
	qer_trans 0.25
	surfaceparm	nolightmap
	surfaceparm	nomarks
	surfaceparm	trans
	nopicmip
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_rec_orange.png
		blendFunc add
	}
}

textures/wsw_flareshalos/flare_sphere_orange
{
	qer_editorimage textures/wsw_flareshalos/flare_sphere_orange.png
	qer_trans 0.25
	cull back
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	nopicmip
	deformVertexes autosprite
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/flare_sphere_orange.png
		blendFunc add
	}
}

textures/wsw_flareshalos/flare_sphere_cyan
{
	qer_editorimage textures/wsw_flareshalos/flare_sphere_cyan.png
	qer_trans 0.25
	cull back
	nopicmip
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	deformVertexes autosprite
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/flare_sphere_cyan.png
		blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
		rgbgen wave distanceramp 0 1 150 600
	}
}

textures/wsw_flareshalos/flare_sphere_cyan_softer
{
	qer_editorimage textures/wsw_flareshalos/flare_sphere_cyan.png
	qer_trans 0.25
	cull back
	nopicmip
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	deformVertexes autosprite
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/flare_sphere_cyan.png
		blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
		rgbgen wave distanceramp 0 0.95 150 600
	}
}

textures/wsw_flareshalos/flare_sphere_fog_white
{
	qer_editorimage textures/wsw_flareshalos/flare_sphere_fog_white.png
	qer_trans 0.25
	cull back
	nopicmip
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	deformVertexes autosprite
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/flare_sphere_fog_white.png
		blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
		rgbgen wave distanceramp 0 1 100 800
	}
}

textures/wsw_flareshalos/glow_neontube_purple
{
	qer_editorimage textures/wsw_flareshalos/glow_neontube_purple.png
	qer_trans 0.25
	cull back
	nopicmip
	surfaceparm	nomarks
	surfaceparm trans
	surfaceparm nonsolid
	deformVertexes autosprite2
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_neontube_purple.png
		blendFunc add
	}
}

textures/wsw_flareshalos/glow_neontube_blue
{
	qer_editorimage textures/wsw_flareshalos/glow_neontube_blue.png
	qer_trans 0.25
	cull back
	nopicmip
	surfaceparm	nomarks
	surfaceparm trans
	surfaceparm nonsolid
	deformVertexes autosprite2
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_neontube_blue.png
		blendFunc add
	}
}

textures/wsw_flareshalos/glow_ellipse_white1
{	
	nopicmip
	qer_editorimage textures/wsw_flareshalos/glow_ellipse_white1.png
	qer_trans 0.25
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nolightmap
	qer_trans 0.5
	deformVertexes autosprite2
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_ellipse_white1.png
		blendFunc add
	}
}


textures/wsw_flareshalos/flare_sphere_white
{
	qer_editorimage textures/wsw_flareshalos/flare_sphere_white.png
	qer_trans 0.25
	cull back
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nonsolid
	deformVertexes autosprite
	nopicmip
	softParticle
	
	{
		detail
		clampmap textures/wsw_flareshalos/flare_sphere_white.png
		blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
	}
}

textures/wsw_flareshalos/glow_halo_white
{
	qer_editorimage textures/wsw_flareshalos/glow_halo_white.png
	//qer_trans 0.25
	cull none
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite2
	nopicmip
	softParticle
	
	{
		detail
		clampmap textures/wsw_flareshalos/glow_halo_white.png
		blendfunc add
		rgbgen wave distanceramp 0 0.7 80 400
	}
}

textures/wsw_flareshalos/glow_halo_white_soft
{
	qer_editorimage textures/wsw_flareshalos/glow_halo_white.png
	qer_trans 0.35
	cull none
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite2
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_halo_white.png
		blendfunc add
		rgbgen wave distanceramp 0 0.5 80 400
	}
}

textures/wsw_flareshalos/glow_halo_blue2
{
	qer_editorimage textures/wsw_flareshalos/glow_halo_blue2.png
	//qer_trans 0.25
	cull none
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite2
	nopicmip
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_halo_blue2.png
		blendfunc add
		rgbgen wave distanceramp 0 0.7 80 400
	}
}

textures/wsw_flareshalos/glow_cone_white
{
	qer_editorimage textures/wsw_flareshalos/glow_cone_white.png
	qer_trans 0.25
	cull none
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite2
	nopicmip
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_cone_white.png
		blendFunc add
		rgbgen wave distanceramp 0 0.5 20 350
	}
}

textures/wsw_flareshalos/glow_cone_cyan
{
	qer_editorimage textures/wsw_flareshalos/glow_cone_cyan.png
	qer_trans 0.25
	cull none
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite2
	nopicmip
	softParticle
	
	{
		detail
		clampmap textures/wsw_flareshalos/glow_cone_cyan.png
		blendFunc add
		rgbgen wave distanceramp 0 0.5 30 350
	}
}

textures/wsw_flareshalos/glow_cone_verycyan
{
	qer_editorimage textures/wsw_flareshalos/glow_cone_verycyan.png
	qer_trans 0.25
	cull none
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite2
	nopicmip
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_cone_verycyan.png
		blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
		rgbgen wave distanceramp 0 0.6 82 500
	}
}

textures/wsw_flareshalos/glow_neontube_red
{
	qer_editorimage textures/wsw_flareshalos/glow_neontube_red.png
	qer_trans 0.25
	cull back
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	nopicmip
	deformVertexes autosprite2
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_neontube_red.png
		blendFunc add
	}
}

textures/wsw_flareshalos/trim_glow_white
{
	qer_editorimage textures/wsw_flareshalos/trim_glow_white.png
	qer_trans 0.25
	surfaceparm	nolightmap
	surfaceparm	nomarks
	surfaceparm	trans
	surfaceparm	nonsolid
	nopicmip
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/trim_glow_white.png
		rgbgen wave distanceramp 0 1 150 600
		blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
		tcmod scroll 0.002 0
	}
}

textures/wsw_flareshalos/trim_glow_blue
{
	qer_editorimage textures/wsw_flareshalos/trim_glow_blue.png
	qer_trans 0.25
	surfaceparm	nolightmap
	surfaceparm	nomarks
	surfaceparm	trans
	surfaceparm	nonsolid
	nopicmip
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/trim_glow_blue.png
		rgbgen wave distanceramp 0 1 150 600
		blendFunc GL_ONE GL_ONE_MINUS_SRC_COLOR
		tcmod scroll 0.002 0
	}
}

textures/wsw_flareshalos/small_light_halo
{
	qer_editorimage textures/wsw_flareshalos/small_light_halo.png
	qer_trans 0.25
	cull none
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite2
	nopicmip
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/small_light_halo.png
		blendFunc add
		rgbgen wave distanceramp 0 0.5 30 400
	}
}

textures/wsw_flareshalos/small_light_halo_blue
{
	qer_editorimage textures/wsw_flareshalos/small_light_halo_blue.png
	qer_trans 0.25
	cull none
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite2
	nopicmip
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/small_light_halo_blue.png
		blendFunc add
		rgbgen wave distanceramp 0 0.5 30 400
	}
}

textures/wsw_flareshalos/glow_halo_white_flat
{
	surfaceparm	nolightmap
	surfaceparm	nomarks
	surfaceparm	trans
	surfaceparm	nonsolid
	nopicmip
	cull front
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_halo_white_flat.png
		rgbgen wave distanceramp 0 1 75 400
		blendFunc GL_SRC_ALPHA GL_ONE
	}
}

textures/wsw_flareshalos/glow_halo_blue2_flat
{
	qer_editorimage textures/wsw_flareshalos/glow_halo_blue2_flat.png
	surfaceparm	nolightmap
	surfaceparm	nomarks
	surfaceparm	trans
	surfaceparm	nonsolid
	nopicmip
	cull front
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/glow_halo_blue2_flat.png
		rgbgen wave distanceramp 0 1 75 400
		blendFunc GL_SRC_ALPHA GL_ONE
	}
}

textures/wsw_flareshalos/flare_sphere_white_front
{
	qer_editorimage textures/wsw_flareshalos/flare_sphere_white.png
	qer_trans 0.75
	cull back
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nonsolid
	nopicmip
	deformVertexes autosprite
	softParticle
	
	{
		detail
		clampmap textures/wsw_flareshalos/flare_sphere_white.png
		rgbgen wave distanceramp 0 2 20 1300
		blendFunc GL_ONE GL_ONE
	}
}

textures/wsw_flareshalos/flare_sphere_white_front_soft
{
	qer_editorimage textures/wsw_flareshalos/flare_sphere_white.png
	qer_trans 0.75
	cull back
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nonsolid
	nopicmip
	deformVertexes autosprite
	softParticle

	{
		detail
		clampmap textures/wsw_flareshalos/flare_sphere_white.png
		rgbgen wave distanceramp 0 1 10 100
		blendFunc GL_ONE GL_ONE
	}
}
