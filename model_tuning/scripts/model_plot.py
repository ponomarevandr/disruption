import matplotlib.pyplot as plt


def formatted(value):
	if isinstance(value, float):
		return '{:.6e}'.format(value)
	return '{}'.format(value)

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
	plt.legend(['$t = {:.3e}$'.format(model.ts[i]) for i in t_indices],
		bbox_to_anchor=(1.02, 1), loc="upper left")
	if print_params:
		plt.figtext(0, 0, (
				'eps_0 = {}\n' +
				'delta = {}\n' +
				'l = {}\n' +
				'Gamma = {}\n' +
				'm = {}\n' +
				'Phi_gradient = {}'
			).format(
				formatted(model.params['eps_0']),
				formatted(model.params['delta']),
				formatted(model.params['l']),
				formatted(model.params['Gamma']),
				formatted(model.params['m']),
				formatted(model.params['Phi_gradient'])
			),
			va='top'
		)
		plt.figtext(0.5, 0, (
				'x_grid = {}\n' +
				't_grid = {}\n' +
				'dx = {}\n' +
				'dt = {}\n' +
				'width = {}\n' +
				'duration = {}'
			).format(
				formatted(model.params['x_grid']),
				formatted(model.params['t_grid']),
				formatted(model.params['dx']),
				formatted(model.params['dt']),
				formatted(model.params['width']),
				formatted(model.params['duration'])
			),
			va='top'
		)
	plt.show()
