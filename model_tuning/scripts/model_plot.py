import matplotlib.pyplot as plt


def model_plot(model, times, print_params=True, **kwargs):
	if 'figsize' in kwargs:
		plt.figure(figsize=kwargs['figsize'])
	if 'title' in kwargs:
		plt.title(kwargs['title'])
	if 'xlim' in kwargs:
		plt.xlim(kwargs['xlim'])
	if 'ylim' in kwargs:
		plt.ylim(kwargs['ylim'])
	plt.xlabel('$x$')
	plt.ylabel(r'$\phi$')
	t_indices = [model.t_index(t) for t in times]
	for i in t_indices:
	    plt.plot(model.xs, model.phi[i])
	#plt.legend(['$t = {:.4f}$'.format(i * model.params['dt_data']) for i in t_indices])
	plt.legend(['$t = {:.2f}$'.format(i * model.params['dt_data']) for i in t_indices],
		bbox_to_anchor=(1.02, 1), loc="upper left")
	if print_params:
		plt.figtext(0, 0, (
				'eps_0 = {:.6e}\n' +
				'delta = {:.6e}\n' +
				'l = {:.6e}\n' +
				'Gamma = {:.6e}\n' +
				'm = {:.6e}\n' +
				'Phi_gradient = {:.6e}'
			).format(
				model.params['eps_0'],
				model.params['delta'],
				model.params['l'],
				model.params['Gamma'],
				model.params['m'],
				model.params['Phi_gradient']
			),
			va='top'
		)
		plt.figtext(0.5, 0, (
				'x_grid = {}\n' +
				't_grid = {}\n' +
				'dx = {:.6e}\n' +
				'dt = {:.6e}\n' +
				'width = {:.6e}\n' +
				'duration = {:.6e}'
			).format(
				model.params['x_grid'],
				model.params['t_grid'],
				model.params['dx'],
				model.params['dt'],
				model.params['width'],
				model.params['duration']
			),
			va='top'
		)
	plt.show()
