#!/usr/bin/perl -w

use Cwd;

$package = "ThrScan";
$maindir = getcwd();

$try = 700;

open(FILE,"<file_$package.lst");
@filelist = <FILE>;
close(FILE);

$nfiles_per_job = 16; # 931 full scan files
$njobs = 60;

$rundir = sprintf("${maindir}/runhist_${package}_try%03d",${try});
mkdir $rundir;

$jobname = sprintf("hist_${package}_try%03d",${try});

for ($irun=0; $irun<$njobs; $irun++){

	$wrkdir = "${rundir}/wrk_${irun}";
	mkdir $wrkdir;

	chdir $wrkdir;
	open(FILE, ">condor");
	print FILE "Universe = vanilla\n";
	print FILE "Accounting_Group = group_alice\n";
	print FILE "Notification = Never\n";
	print FILE "Getenv = true\n";
	print FILE "Priority = +70\n";
	print FILE "Executable = jobscript\n";
	print FILE "JobBatchName = ${jobname}\n";
	print FILE "Log = jobscript.log\n";
	print FILE "Output = jobscript.out\n";
	print FILE "Error = jobscript.err\n";
	print FILE "Queue\n";
	close(FILE);
	
	open(FILE,">flist.txt");
	for ($iseg=0; $iseg<$nfiles_per_job; $iseg++){
		$filename = $filelist[$nfiles_per_job*$irun+$iseg];
		print FILE $filename;
	}
	close(FILE);

	open(FILE, ">jobscript");
	print FILE "#!/bin/bash\n";

	print FILE "cp -av ${maindir}/ScanLayer1.C .\n";
	print FILE "root -l -b -q 'ScanLayer1.C'\n";

	$filename = sprintf("%s/Histograms_%s_%05d.root",$rundir,$package,$irun);
	print FILE "mv -v outfile_Layers.root $filename\n\n";

	# print FILE "rm -rf *.C\n\n";

	close(FILE);
	chmod 0755, "jobscript";

	system "condor_submit condor";

}
