/*
 * Created by SharpDevelop.
 * User: Bernard
 * Date: 2011/3/29
 * Time: 10:32
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.IO;

namespace DeleteComma
{
	class Program
	{
		public static void Main(string[] args)
		{
			for(int i = 10; i <= 720; i += 10){
				string inFile = @"D:\sigDist\sigDist_"+i+"cm.dat";
				string outFile = @"D:\sigDist\sig_dist_"+i+"cm.dat";
				
				StreamReader infile = new StreamReader(inFile);
				if (!File.Exists(outFile)){
	               FileStream fs = File.Create(outFile);
	               fs.Close();
	           }
	           StreamWriter outfile = new StreamWriter(outFile);
	           
	           for(int ii = 0; ii < 100; ii++){
		           string line = infile.ReadLine();
		           string[] lineSplit = line.Split(new string[] { "," }, StringSplitOptions.None);

	          		outfile.WriteLine(lineSplit[0]+","+lineSplit[1]+","+lineSplit[2]+","+lineSplit[3]);
	           }
	           infile.Close();
	          	outfile.Close();
			}
			
			Console.Write("Press any key to continue . . . ");
			Console.ReadKey(true);
		}
	}
}