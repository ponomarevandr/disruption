import numpy as np
from vtk import vtkXMLImageDataReader
from vtk.util.numpy_support import vtk_to_numpy

from glob import glob
import os.path


class VTKReader:
	def __init__(self, content_dir):
		self.content_dir = content_dir
		self.phi = []

	def _detect_files(self):
		self.files = glob(os.path.join(self.content_dir, 'out_*[0-9].vti'))
		print('Detected {} vti files'.format(len(self.files)))
		self.files = sorted([(len(file_name), file_name) for file_name in self.files])
		self.files = [file_name for (_, file_name) in self.files]

	def load(self):
		self._detect_files()
		for file in self.files:
			reader = vtkXMLImageDataReader()
			reader.SetFileName(file)
			reader.Update()
			output = reader.GetOutput()
			self.dims = np.array(output.GetDimensions())
			self.phi.append(self._read_array(output, 'Phase_field'))
		self.phi = np.stack(self.phi)
		print('Loading done!')

	def _read_array(self, output, name):
		array_vtk = output.GetCellData().GetArray(name)
		array_np = vtk_to_numpy(array_vtk).reshape(self.dims - 1, order='F')
		return array_np
