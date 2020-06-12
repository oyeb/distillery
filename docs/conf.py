# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import sys
# sys.path.insert(0, os.path.abspath('.'))

import os
import subprocess
def configureDoxyfile(input_dir, output_dir):
  with open('Doxyfile.in', 'r') as file :
    filedata = file.read()
    
  filedata = filedata.replace('@DOXYGEN_INPUT_DIR@', input_dir)
  filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', output_dir)
    
  with open('Doxyfile', 'w') as file:
    file.write(filedata)

# Check if we're running on Read the Docs' servers
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

if read_the_docs_build:
  input_dir = '../'
  output_dir = 'build'
  configureDoxyfile(input_dir, output_dir)
  subprocess.call('doxygen', shell=True)

# -- Project information -----------------------------------------------------

project = 'Distillery'
copyright = '2020, Ajeesh Kumar, Ananya Bahadur'
author = 'Ajeesh Kumar, Ananya Bahadur'

# The full version, including alpha/beta/rc tags
release = '0.0.1'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
  'breathe',
  'cloud_sptheme',

  # standard sphinx extensions
  'sphinx.ext.autodoc',
  'sphinx.ext.todo',

  # 3rd party extensions
  'sphinxcontrib.fulltoc',
  'sphinx.ext.viewcode',  # mainly to make sure layout works properly
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'cloud'
html_theme_options = {
  "roottarget": "index",
  "borderless_decor": True,
}

html_sidebars = {'**': ['searchbox.html', 'globaltoc.html']}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# Breathe Configuration
breathe_default_project = "Distillery"

# The master toctree document.
master_doc = 'toc'

# The frontpage document.
index_doc = 'index'
