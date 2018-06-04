import xml.etree.cElementTree as eltree
from xml.dom import minidom
import collections

class PipelineManifest(object):
    """
    Class for creating the pipeline data product manifest 
    """
    def __init__(self, ouss_id):
        self.ouss_id = ouss_id
	self.piperesults = eltree.Element("piperesults", name=ouss_id)

    def import_xml(self, xmlfile):
        """
        Import the manifest from an existing manifest file
        """
        with open(xmlfile, 'r') as f:
            lines = map(lambda x: x.replace('\n', '').strip(), f.readlines())
            self.piperesults = eltree.fromstring(''.join(lines))

    def set_ous(self, ous_name):
        """
        Set an OUS element and return it 
	"""
        return eltree.SubElement(self.piperesults, "ous", name=ous_name)

    def get_ous(self):
        """
        Currently this assumes there is only one ous as is the case
        for member ous processing
        """
        return self.piperesults.getchildren()[0]

    def add_casa_version (self, ous, casa_version):
        """
        Set the CASA version
        """
	eltree.SubElement (ous, "casaversion", name=casa_version)

    def add_pipeline_version (self, ous, pipeline_version):
        """
        Set the pipeline version
        """
	eltree.SubElement (ous, "pipeline_version", name=pipeline_version)

    def add_procedure_name (self, ous, procedure_name):
        """
        Set the procedure name
        """
	eltree.SubElement (ous, "procedure_name", name=procedure_name)

    def set_session (self, ous, session_name):
	"""
        Set a SESSION element in an OUS element and return it 
	"""
	return eltree.SubElement (ous, "session", name=session_name)

    def get_session (self, ous, session_name):
	"""
        Get a SESSION element in an OUS element and return it 
	"""
        for session in ous.iter('session'):
            if session.attrib['name'] == session_name:
                return session
        return None

    def get_asdm (self, session, asdm_name):
	"""
        Get an ASDM element in a SESSION element and return it 
	"""
        for asdm in seesion.iter('asdm'):
            if asdm.attrib['name'] == asdm_name:
                return sdm
        return None


    def add_caltables (self, session, caltables_file):
        eltree.SubElement(session, "caltables", name=caltables_file)

    def add_auxcaltables (self, session, caltables_file):
        eltree.SubElement(session, "aux_caltables", name=caltables_file)

    def get_caltables (self, ous):
        caltables_dict = collections.OrderedDict()
        for session in ous.iter('session'):
            for caltable in session.iter('caltables'):
                caltables_dict[session.attrib['name']] = caltable.attrib['name']
        return caltables_dict

    def add_ms (self, session, asdm_name, ms_file):
	"""
        Add an alternative ASDM element to a SESSION element
	"""
        asdm = eltree.SubElement (session, "asdm", name=asdm_name)
	eltree.SubElement(asdm, "finalms", name=ms_file)

    def add_asdm (self, session, asdm_name, flags_file, calapply_file):
	"""
        Add an ASDM element to a SESSION element
	"""
        asdm = eltree.SubElement (session, "asdm", name=asdm_name)
	eltree.SubElement(asdm, "finalflags", name=flags_file)
	eltree.SubElement(asdm, "applycmds", name=calapply_file)

    def add_asdm_imlist (self, session, asdm_name, flags_file, calapply_file, imagelist, type):
	"""
        Add an ASDM element to a SESSION element
	"""
        asdm = eltree.SubElement (session, "asdm", name=asdm_name)
	eltree.SubElement(asdm, "finalflags", name=flags_file)
	eltree.SubElement(asdm, "applycmds", name=calapply_file)
        for image in imagelist:
	    eltree.SubElement(asdm, "image", name=image, imtype=type)

    def add_auxasdm (self, session, asdm_name, calapply_file):
	"""
        Add an ASDM element to a SESSION element
	"""
        asdm = eltree.SubElement (session, "aux_asdm", name=asdm_name)
	eltree.SubElement(asdm, "applycmds", name=calapply_file)

    def get_final_flagversions (self, ous):
        """
        Get a list of the final flag versions
        """
        finalflags_dict = collections.OrderedDict()
        for session in ous.iter('session'):
            for asdm in session.iter('asdm'):
                for finalflags in asdm.iter('finalflags'): 
                    finalflags_dict[asdm.attrib['name']] = finalflags.attrib['name']
        return finalflags_dict

    def get_applycals (self, ous):
        """
        Get a list of the final applycal instructions
        """
        applycmds_dict = collections.OrderedDict()
        for session in ous.iter('session'):
            for asdm in session.iter('asdm'):
                for applycmds in asdm.iter('applycmds'): 
                    applycmds_dict[asdm.attrib['name']] = applycmds.attrib['name']
        return applycmds_dict

    def add_pprfile(self, ous, ppr_file):
	"""
        Add the pipeline processing request file to the OUS element
	"""
        eltree.SubElement (ous, "piperequest", name=ppr_file)

    def add_images(self, ous, imagelist, type):
	"""
        Add a list of images to the OUS element. Note that this does not have
        to be an ous element, e.d. an asdm element will do
	"""
        for image in imagelist:
	    eltree.SubElement(ous, "image", name=image, imtype=type)

    def add_pipescript(self, ous, pipescript):
	"""
        Add the pipeline processing script to the OUS element
	"""
        eltree.SubElement (ous, "pipescript", name=pipescript)

    def add_restorescript(self, ous, restorescript):
	"""
        Add the pipeline restore script to the OUS element
	"""
        eltree.SubElement (ous, "restorescript", name=restorescript)

    def add_weblog (self, ous, weblog):
	"""
        Add the weblog to the OUS element
	"""
        eltree.SubElement (ous, "weblog", name=weblog)

    def add_casa_cmdlog (self, ous, casa_cmdlog):
	"""
        Add the CASA commands log to the OUS element
	"""
        eltree.SubElement (ous, "casa_cmdlog", name=casa_cmdlog)

    def add_flux_file (self, ous, flux_file):
	"""
        Add the flux file to the OUS element
        Remove at some point.
	"""
        eltree.SubElement (ous, "flux_file", name=flux_file)

    def add_antennapos_file (self, ous, antennapos_file):
	"""
        Add the antenna positions file to the OUS element
        Remove at some point
	"""
        eltree.SubElement (ous, "antennapos_file", name=antennapos_file)

    def add_cont_file (self, ous, cont_file):
	"""
        Add the continuum frequency ranges file to the OUS element
        Remove at some point
	"""
        eltree.SubElement (ous, "cont_file", name=cont_file)

    def add_aux_products_file (self, ous, auxproducts_file):
	"""
        Add the auxiliary products file. Is one enough ?
	"""
        eltree.SubElement (ous, "aux_products_file", name=auxproducts_file)

    def add_aqua_report (self, ous, aqua_report):
	"""
        Add the AQUA report to the OUS element
	"""
        eltree.SubElement (ous, "aqua_report", name=aqua_report)

    def write (self, filename):
	"""
        Convert the document to a nicely formatted XML string
	and save it in a file
	"""
        xmlstr = eltree.tostring(self.piperesults, 'utf-8')

	# Reformat it to prettyprint style
	reparsed = minidom.parseString(xmlstr)
	reparsed_xmlstr = reparsed.toprettyxml(indent="  ")

	# Save it to a file.
        with open (filename, "w") as manifestfile:
            manifestfile.write(reparsed_xmlstr)

