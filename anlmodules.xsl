<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
  <xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN" doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"/>
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
	      <title><xsl:value-of select="anl_list/title" /></title>
	      <link rel="stylesheet" type="text/css">
          <xsl:attribute name="href"><xsl:value-of select="anl_list/css/@href" /></xsl:attribute>
        </link>
      </head>
      <body>
        <h1><xsl:value-of select="anl_list/title" /></h1>
	      <xsl:apply-templates select="anl_list" mode="list" />
	      <xsl:apply-templates select="anl_list" mode="detail" />
	    </body>
    </html>
  </xsl:template>
  <xsl:template match="anl_list" mode="list">
    <h2>List of modules</h2>
    <xsl:apply-templates select="anl_module" mode="list" />
  </xsl:template>
  <xsl:template match="anl_list" mode="detail">
    <xsl:apply-templates select="anl_module" mode="detail" />
  </xsl:template>
  <xsl:template match="anl_module" mode="list">
    <xsl:variable name="mod" select="document(@href)" />
    <xsl:apply-templates select="$mod//category" mode="h3" />
    <ul>
      <xsl:apply-templates select="$mod//module" mode="list" />
    </ul>
  </xsl:template>
  <xsl:template match="anl_module" mode="detail">
    <xsl:variable name="mod" select="document(@href)" />
    <xsl:apply-templates select="$mod//category" mode="h2" />
    <xsl:apply-templates select="$mod//module" mode="detail" />
  </xsl:template>
  <xsl:template match="category" mode="h2">
    <h2><xsl:value-of select="." /></h2>
  </xsl:template>
  <xsl:template match="category" mode="h3">
    <h3><xsl:value-of select="." /></h3>
  </xsl:template>
  <xsl:template match="module" mode="list" >
    <xsl:variable name="module_name" select="name" />
    <li>
      <a>
	      <xsl:attribute name="href">#<xsl:value-of select="$module_name" /></xsl:attribute>
	      <xsl:value-of select="$module_name" />
      </a>
    </li>
  </xsl:template>
  <xsl:template match="module" mode="detail" >
    <xsl:apply-templates />
  </xsl:template>
  <xsl:template match="name">
    <xsl:variable name="name_id" select="." />
    <h3>
      <xsl:attribute name="id"><xsl:value-of select="$name_id" /></xsl:attribute>
      <xsl:value-of select="." />
    </h3>
  </xsl:template>
  <xsl:template match="version">
    <p>Version: <xsl:value-of select="." /></p>
  </xsl:template>
  <xsl:template match="super">
    <xsl:variable name="super_id" select="." />
    <p>Derived from:
    <span style="font-weight: bold;">
	    <a>
	      <xsl:attribute name="href">#<xsl:value-of select="$super_id" /></xsl:attribute>
	      <xsl:value-of select="$super_id" />
	    </a>
    </span>
    </p>
  </xsl:template>
  <xsl:template match="sa">
    <xsl:variable name="module_id" select="." />
    <p>See also: 
    <span style="font-weight: bold;">
	    <a>
	      <xsl:attribute name="href">#<xsl:value-of select="$module_id" /></xsl:attribute>
	      <xsl:value-of select="$module_id" />	
	    </a>
    </span>
    </p>
  </xsl:template>
  <xsl:template match="text">
    <p><xsl:value-of select="." /></p>
  </xsl:template>
  <xsl:template match="parameters">
    <table frame="border" rules="all">
      <caption class="mod_param_cap">Module parameters</caption>
      <colgroup><col width="20%" /><col width="10%" /><col width="10%" /><col width="10%" /><col width="50%" /></colgroup>
      <thead>
	      <tr>
	        <th>Name</th>
	        <th>Type</th>
	        <th>Unit</th>
	        <th>Default value</th>
	        <th>Description</th>
	      </tr>
      </thead>
      <tbody>
	      <xsl:apply-templates select="param" />
      </tbody>
    </table>
  </xsl:template>
  <xsl:template match="param">
    <tr>
      <xsl:if test="string-length(@container_type)&gt;0">
	      <xsl:attribute name="class">container_type_<xsl:value-of select="@container_type" /></xsl:attribute>
      </xsl:if>
      <td><xsl:value-of select="name" /></td>
      <td><xsl:value-of select="type" /><xsl:if test="string-length(@container_type)&gt;0"> [<xsl:value-of select="@container_type" />]</xsl:if>
      </td>
      <td><xsl:value-of select="unit" /></td>
      <td><xsl:value-of select="default_value" /></td>
      <td><xsl:value-of select="description" /></td>
    </tr>
  </xsl:template>
</xsl:stylesheet>
