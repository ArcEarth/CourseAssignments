using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace TripAdviserDesktopClient
{
	/// <summary>
	/// Interaction logic for App.xaml
	/// </summary>
	public partial class App : Application
	{
		static public string DatabaseConnectionString = @"Data Source=(localdb)\ProjectsV12;Initial Catalog=TripAdviserDatabase;Integrated Security=True;Connect Timeout=15;Encrypt=False;TrustServerCertificate=False";
		static public TripAdviserDataContext DataContext = new TripAdviserDataContext(DatabaseConnectionString);
	}
}
