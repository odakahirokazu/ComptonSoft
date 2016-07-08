<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">

  <xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN" doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"/>

  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
	<title>Data</title>
	<style type="text/css">
          html {background-color: #FFFFDD; color: #000000;}
	  body {padding: 3%;}
	  h1 {text-align: center;}
	  .sd {border-top: solid 1px; border-left: solid 8px; padding: 4px; }
	  .comment {color: #663300;}
	  .totalnum {color: #FF0000;}
	  .com_param {background-color: #FFDDCC;}
	  .col_pha {background-color: #DDDDFF;}
	  .col_dif {background-color: #DDFFDD;}
	  .special_param {color: #0000FF; }
	  table {margin:1em; border-collapse: collapse; border: 2px solid #000000;}
	  thead {border: 2px solid #000000; text-align: center;}
	  tbody {border: 2px solid #000000; text-align: right;}
	  th, td {border: 1px solid #000000;}
	</style>
      </head>

      <body>
	<h1>Detector Simulation Parameters (Compton Soft version 4.5)</h1>
	<p>Detector Name: <xsl:value-of select="configuration/name" /></p>
	<xsl:apply-templates select="configuration/comment" />
	<xsl:apply-templates select="configuration/cce_file" />
	<xsl:apply-templates select="configuration/auto_position" />
	<xsl:apply-templates select="configuration/sd_check" />
	<p>Total Sensitive Detectors = 
	<span class="totalnum"><xsl:value-of select="configuration/sensitive_detectors/@total" /></span>
	</p>
	<xsl:apply-templates select="configuration/sensitive_detectors/sensitive_detector" />
      </body>
    </html>
  </xsl:template>

  <xsl:template match="comment">
    <p>COMMENT: <span class="comment"><xsl:value-of select="." /></span></p>
  </xsl:template>

  <xsl:template match="cce_file">
    <p>CCE file: <xsl:value-of select="." /></p>
  </xsl:template>

  <xsl:template match="auto_position">
    <p>Auto Position Mode: ON</p>
  </xsl:template>

  <xsl:template match="sd_check">
    <p>SD Check: ON</p>
  </xsl:template>

  <xsl:template match="sensitive_detector">
    <h2 class="sd"><xsl:value-of select="name" /></h2>
    <p class="setting">Type: <xsl:value-of select="type" /></p>
    <table>
      <colgroup><col /></colgroup>
      <colgroup><col /></colgroup>
      <colgroup><col /></colgroup>
      <colgroup class="col_pha"><col /><col /><col /><col /><col /><col /><col /><col /><col /><col /></colgroup>
      <colgroup class="col_dif"><col /><col /><col /></colgroup>
      <tr> 
	<th>ID</th>
	<th>Key</th>
	<th>Upside</th>
	<th>Sim PHA</th>
	<th>CCE map</th>
	<th>Bias [V]</th>
	<th>E field</th>
	<th>P0</th>
	<th>P1</th>
	<th>P2</th>
	<th>P3</th>
	<th>MuTau e [cm2/V]</th>
	<th>MuTau h [cm2/V]</th>
	<th>Sim Diffusiton</th>
	<th>anode</th>
	<th>cathode</th>
      </tr>
      
      <xsl:variable name="upside_anode" select="simulation_param_common/upside_anode/@set" />
      <xsl:variable name="upside_pad" select="simulation_param_common/upside_pad/@set" />
      <xsl:variable name="upside_xstrip" select="simulation_param_common/upside_xstrip/@set" />
      <xsl:variable name="sim_pha_mode" select="simulation_param_common/sim_pha/@mode" />
      <xsl:variable name="cce_map" select="simulation_param_common/sim_pha/@map" />
      <xsl:variable name="bias" select="simulation_param_common/sim_pha/bias" />
      <xsl:variable name="efield_mode" select="simulation_param_common/sim_pha/efield/@mode" />
      <xsl:variable name="efield_p0" select="simulation_param_common/sim_pha/efield/@param0" />
      <xsl:variable name="efield_p1" select="simulation_param_common/sim_pha/efield/@param1" />
      <xsl:variable name="efield_p2" select="simulation_param_common/sim_pha/efield/@param2" />
      <xsl:variable name="efield_p3" select="simulation_param_common/sim_pha/efield/@param3" />
      <xsl:variable name="mutau_e" select="simulation_param_common/sim_pha/mutau_electron" />
      <xsl:variable name="mutau_h" select="simulation_param_common/sim_pha/mutau_hole" />
      <xsl:variable name="sim_diffusion_mode" select="simulation_param_common/sim_diffusion/@mode" />
      <xsl:variable name="dif_factor_anode" select="simulation_param_common/sim_diffusion/spread_factor_anode" />
      <xsl:variable name="dif_factor_cathode" select="simulation_param_common/sim_diffusion/spread_factor_cathode" />
      <xsl:variable name="dif_const_anode" select="simulation_param_common/sim_diffusion/constant_anode" />
      <xsl:variable name="dif_const_cathode" select="simulation_param_common/sim_diffusion/constant_cathode" />
      
      <tr class="com_param">
	<th>COM</th>
	<td></td>
	<td>
	  <xsl:call-template name="disp_upside">
	    <xsl:with-param name="upside_anode" select="$upside_anode" />
	    <xsl:with-param name="upside_pad" select="$upside_pad" />
	    <xsl:with-param name="upside_xstrip" select="$upside_xstrip" />
	  </xsl:call-template> 
	</td>
	<td><xsl:value-of select="$sim_pha_mode" /></td>
	<td><xsl:value-of select="$cce_map" /></td>
	<td><xsl:value-of select="$bias" /></td>
	<td><xsl:value-of select="$efield_mode" /></td>
	<td><xsl:value-of select="$efield_p0" /></td>
	<td><xsl:value-of select="$efield_p1" /></td>
	<td><xsl:value-of select="$efield_p2" /></td>
	<td><xsl:value-of select="$efield_p3" /></td>
	<td><xsl:value-of select="$mutau_e" /></td>
	<td><xsl:value-of select="$mutau_h" /></td>
	<td><xsl:value-of select="$sim_diffusion_mode" /></td>
	<xsl:choose>
	  <xsl:when test="$sim_diffusion_mode=1">
	    <td><xsl:value-of select="$dif_factor_anode" /></td>
	    <td><xsl:value-of select="$dif_factor_cathode" /></td>
	  </xsl:when>
	  <xsl:when test="$sim_diffusion_mode=2">
	    <td><xsl:value-of select="$dif_const_anode" /> um</td>
	    <td><xsl:value-of select="$dif_const_cathode" /> um</td>
	  </xsl:when>
	  <xsl:otherwise>
	    <td></td><td></td>
	  </xsl:otherwise>
	</xsl:choose>
      </tr>

      <xsl:apply-templates select="detector_list/detector">
	<xsl:with-param name="com_upside_anode" select="$upside_anode" />
	<xsl:with-param name="com_upside_pad" select="$upside_pad" />
	<xsl:with-param name="com_upside_xstrip" select="$upside_xstrip" />
	<xsl:with-param name="com_sim_pha_mode" select="$sim_pha_mode" />
	<xsl:with-param name="com_cce_map" select="$cce_map" />
	<xsl:with-param name="com_bias" select="$bias" />
	<xsl:with-param name="com_efield_mode" select="$efield_mode" />
	<xsl:with-param name="com_efield_p0" select="$efield_p0" />
	<xsl:with-param name="com_efield_p1" select="$efield_p1" />
	<xsl:with-param name="com_efield_p2" select="$efield_p2" />
	<xsl:with-param name="com_efield_p3" select="$efield_p3" />
	<xsl:with-param name="com_mutau_e" select="$mutau_e" />
	<xsl:with-param name="com_mutau_h" select="$mutau_h" />
	<xsl:with-param name="com_sim_diffusion_mode" select="$sim_diffusion_mode" />
	<xsl:with-param name="com_dif_factor_anode" select="$dif_factor_anode" />
	<xsl:with-param name="com_dif_factor_cathode" select="$dif_factor_cathode" />
	<xsl:with-param name="com_dif_const_anode" select="$dif_const_anode" />
	<xsl:with-param name="com_dif_const_cathode" select="$dif_const_cathode" />
      </xsl:apply-templates>
    </table>
  </xsl:template>


  <xsl:template match="detector">
    <xsl:param name="com_upside_anode" select="''" />
    <xsl:param name="com_upside_pad" select="''" />
    <xsl:param name="com_upside_xstrip" select="''" />
    <xsl:param name="com_sim_pha_mode" select="''" />
    <xsl:param name="com_cce_map" select="''" />
    <xsl:param name="com_bias" select="''" />
    <xsl:param name="com_efield_mode" select="''" />
    <xsl:param name="com_efield_p0" select="''" />
    <xsl:param name="com_efield_p1" select="''" />
    <xsl:param name="com_efield_p2" select="''" />
    <xsl:param name="com_efield_p3" select="''" />
    <xsl:param name="com_mutau_e" select="''" />
    <xsl:param name="com_mutau_h" select="''" />
    <xsl:param name="com_sim_diffusion_mode" select="''" />
    <xsl:param name="com_dif_factor_anode" select="''" />
    <xsl:param name="com_dif_factor_cathode" select="''" />
    <xsl:param name="com_dif_const_anode" select="''" />
    <xsl:param name="com_dif_const_cathode" select="''" />

    <xsl:variable name="upside_anode" select="simulation_param/upside_anode/@set" />
    <xsl:variable name="upside_pad" select="simulation_param/upside_pad/@set" />
    <xsl:variable name="upside_xstrip" select="simulation_param/upside_xstrip/@set" />
    <xsl:variable name="sim_pha_mode" select="simulation_param/sim_pha/@mode" />
    <xsl:variable name="cce_map" select="simulation_param/sim_pha/@map" />
    <xsl:variable name="bias" select="simulation_param/sim_pha/bias" />
    <xsl:variable name="efield_mode" select="simulation_param/sim_pha/efield/@mode" />
    <xsl:variable name="efield_p0" select="simulation_param/sim_pha/efield/@param0" />
    <xsl:variable name="efield_p1" select="simulation_param/sim_pha/efield/@param1" />
    <xsl:variable name="efield_p2" select="simulation_param/sim_pha/efield/@param2" />
    <xsl:variable name="efield_p3" select="simulation_param/sim_pha/efield/@param3" />
    <xsl:variable name="mutau_e" select="simulation_param/sim_pha/mutau_electron" />
    <xsl:variable name="mutau_h" select="simulation_param/sim_pha/mutau_hole" />
    <xsl:variable name="sim_diffusion_mode" select="simulation_param/sim_diffusion/@mode" />
    <xsl:variable name="dif_factor_anode" select="simulation_param/sim_diffusion/spread_factor_anode" />
    <xsl:variable name="dif_factor_cathode" select="simulation_param/sim_diffusion/spread_factor_cathode" />
    <xsl:variable name="dif_const_anode" select="simulation_param/sim_diffusion/constant_anode" />
    <xsl:variable name="dif_const_cathode" select="simulation_param/sim_diffusion/constant_cathode" />
    
    <tr>
      <th><xsl:value-of select="@id" /></th>
      <td><xsl:value-of select="@key" /></td>
      
      <xsl:variable name="final_upside_anode">
	<xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_upside_anode" /><xsl:with-param name="special" select="$upside_anode" /></xsl:call-template>
      </xsl:variable>
      <xsl:variable name="final_upside_pad">
	<xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_upside_pad" /><xsl:with-param name="special" select="$upside_pad" /></xsl:call-template>
      </xsl:variable>
      <xsl:variable name="final_upside_xstrip">
	<xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_upside_xstrip" /><xsl:with-param name="special" select="$upside_xstrip" /></xsl:call-template>
      </xsl:variable>

      <td>
	<xsl:choose>
	  <xsl:when test="string-length($upside_anode)=0 and string-length($upside_pad)=0 and string-length($upside_xstrip)=0">
	    <xsl:call-template name="disp_upside">
	      <xsl:with-param name="upside_anode" select="$final_upside_anode" />
	      <xsl:with-param name="upside_pad" select="$final_upside_pad" />
	      <xsl:with-param name="upside_xstrip" select="$final_upside_xstrip" />
	    </xsl:call-template>
	  </xsl:when>
	  <xsl:otherwise>
	    <span class="special_param">
	      <xsl:call-template name="disp_upside">
		<xsl:with-param name="upside_anode" select="$final_upside_anode" />
		<xsl:with-param name="upside_pad" select="$final_upside_pad" />
		<xsl:with-param name="upside_xstrip" select="$final_upside_xstrip" />
	      </xsl:call-template>
	    </span>
	  </xsl:otherwise>
	</xsl:choose>
      </td>
      <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_sim_pha_mode" /><xsl:with-param name="special" select="$sim_pha_mode" /></xsl:call-template></td>
      <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_cce_map" /><xsl:with-param name="special" select="$cce_map" /></xsl:call-template></td>
      <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_bias" /><xsl:with-param name="special" select="$bias" /></xsl:call-template></td>
      <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_efield_mode" /><xsl:with-param name="special" select="$efield_mode" /></xsl:call-template></td>
      <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_efield_p0" /><xsl:with-param name="special" select="$efield_p0" /></xsl:call-template></td>
      <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_efield_p1" /><xsl:with-param name="special" select="$efield_p1" /></xsl:call-template></td>
      <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_efield_p2" /><xsl:with-param name="special" select="$efield_p2" /></xsl:call-template></td>
      <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_efield_p3" /><xsl:with-param name="special" select="$efield_p3" /></xsl:call-template></td>
      <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_mutau_e" /><xsl:with-param name="special" select="$mutau_e" /></xsl:call-template></td>
      <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_mutau_h" /><xsl:with-param name="special" select="$mutau_h" /></xsl:call-template></td>
      <xsl:variable name="final_sim_diffusion_mode">
	<xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_sim_diffusion_mode" /><xsl:with-param name="special" select="$sim_diffusion_mode" /></xsl:call-template>
      </xsl:variable>
      <td><xsl:value-of select="$final_sim_diffusion_mode" /></td>
      <xsl:choose>
	<xsl:when test="$final_sim_diffusion_mode=1">
	  <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_dif_factor_anode" /><xsl:with-param name="special" select="$dif_factor_anode" /></xsl:call-template></td>
	  <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_dif_factor_cathode" /><xsl:with-param name="special" select="$dif_factor_cathode" /></xsl:call-template></td>
	</xsl:when>
	<xsl:when test="$final_sim_diffusion_mode=2">
	  <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_dif_const_anode" /><xsl:with-param name="special" select="$dif_const_anode" /></xsl:call-template> um</td>
	  <td><xsl:call-template name="disp_param"><xsl:with-param name="common" select="$com_dif_const_cathode" /><xsl:with-param name="special" select="$dif_const_cathode" /></xsl:call-template> um</td>
	</xsl:when>
	<xsl:otherwise>
	  <td></td><td></td>
	</xsl:otherwise>
      </xsl:choose>
    </tr>
  </xsl:template>


  <xsl:template name="disp_param">
    <xsl:param name="common" select="''" />
    <xsl:param name="special" select="''" />
    <xsl:choose>
      <xsl:when test="string-length($special)=0"><xsl:value-of select="$common" /></xsl:when>
      <xsl:otherwise><span class="special_param"><xsl:value-of select="$special" /></span></xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="disp_upside">
    <xsl:param name="upside_anode" select="''" />
    <xsl:param name="upside_pad" select="''" />
    <xsl:param name="upside_xstrip" select="''" />
    <xsl:if test="$upside_anode=1">anode</xsl:if><xsl:if test="$upside_anode=0">cathode</xsl:if> /
    <xsl:if test="$upside_pad=1">pad</xsl:if><xsl:if test="$upside_pad=0">com</xsl:if>
    <xsl:if test="$upside_xstrip=1">x-strip</xsl:if><xsl:if test="$upside_xstrip=0">y-strip</xsl:if>
  </xsl:template>

</xsl:stylesheet>
