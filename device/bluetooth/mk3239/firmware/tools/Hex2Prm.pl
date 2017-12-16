#!/bin/perl
#
# This is a tool to convert a configuration data image from intel hex format to patch ram format
# format suitable for direct transmission to a device for 2045B1
# it will strip all unnecessary hex from the intel hex file and just keep the data portion, 
# The hex file ***.prm will be used to modify bcm2045_prm.c BCM2045_PatchRam array to define internal build patch
#
# 
# The Proprietary format is as follows:
#


####################################################################################################
# script body
####################################################################################################
{
	# extract and check the inputs
	my $inputsOk = 1;
    
	if( @ARGV != 1 )
	{
		$inputsOk = 0;
	}
    
	my $inputFileName = shift ARGV;
	$_ = $inputFileName;
	if( !/(.*)(\.hex)/ )
	{
	   $inputsOk = 0;
	}
	
	my $outFileName = $1.".prm"; 
    
	if( !$inputsOk )
	{
		print "Usage: IncBD_ADDR.pl InputFileName.hex\n\n";
    
		print "The output file will be InputFileName.prm for hex file.\n";
		exit 0;
	}
    
	# parse the input file
	ParseHexFile( \@inputRecords, $inputFileName );      # note: \@ is a reference, akin to & in C++
    
	# open the output file
	open( OutputFile, ">$outFileName") || die "Can't open $outFileName for output\n";
   
    my $num = 0;
	foreach $record (@inputRecords)
	{
		my @data = @{$record->{"data"}};
		if($record->{"len"} != ($#data + 1))
		{
			die "Internal error: record length field should not mismatch data array length\n";
		}

        if ($record->{"type"} == 0) {

		my $line;
	    my $append;
     

		foreach $byte (@data)
		{
            $num = $num + 1;
			$append = sprintf "0x%02X, ", $byte;
            
			$line .= $append;
            
            if ($num == 16)
            {
                $append = sprintf "\n";
                $line .= $append;
                $num = 0;
            } 
		}

		print OutputFile $line;
        }
	}
	

	close OutputFile;
}


####################################################################################################
sub ParseHexFile()
####################################################################################################
{
	my $recordArrayRef = shift @_;
	my $filename = shift @_;
	my $line;

	open( inputFile, $filename ) || die "Can't open $filename\n";

	while( $line = <inputFile> )
	{
		# get a line of the file
		chomp $line;
		my $record = {};

		# extract the colon, length of the hex record
		$_ = $line;
		if(! /:([\dA-Fa-f]{2})([\dA-Fa-f]{4})([\dA-Fa-f]{2})/ )
		{
			# doesn't match
			die "Invalid hex input file $filename\n";
		}
		if( defined($`) && ($` ne "") )
		{
			# nothing should be before the colon
			die "Invalid hex input file $filename\n";
		}
		$record->{"len"} = hex $1;
		$record->{"addr"} = hex $2;
		$record->{"type"} = hex $3;
		if( ( ($record->{"type"} != 2) || ($record->{"addr"} < 0x8000)  ||
			($record->{"len"} > 16) ) && ($record->{"type"} != 1) )
		{
			# die "Invalid input file $filename (input file must contain only hex records in\n".
			#	"bank 1/type code 2, with addresses >= 0x8000 and len <= 16 for each hex\n".
			#	"record)\n";
		}

        
		$remainder = $';

		# extract the remaining bytes, including the checksum
		my @data = ();
		my $byteIndex;
		my $checksum = $record->{"len"} + ($record->{"addr"}>>8) + $record->{"addr"} +
			$record->{"type"};
		foreach $byteIndex ( 1 .. ($record->{"len"}+1) )
		{
			$_ = $remainder;
			if(! /([\dA-Fa-f]{2})/ )
			{
				# doesn't match
				die "Invalid hex input file $filename\n";
			}
			if( defined($`) && ($` ne "") )
			{
				# nothing should be before the data byte
				die "Invalid hex input file $filename\n";
			}
			my $byte = hex $1;
			$checksum += $byte;
			push @data, $byte;
			$remainder = $';
		}

		# validate the checksum
		if( ($checksum&0xFF) != 0)
		{
			# nothing should be before the data byte
			die "Invalid hex input file $filename\n";
		}

		# remove the checksum from the data array
		pop @data;
        
		# there should be nothing remaining on the line
		if( defined($remainder) && ($remainder ne "") )
		{
			# nothing should be before the data byte
			die "Invalid hex input file $filename\n";
		}

		if( $record->{"type"} != 1 )
		{
			# store the data array in the record
			$record->{"data"} = \@data;         # note: \@ obtains a reference to the data array

			# store the record
			push @$recordArrayRef , $record;    # note: using a hash with $ appears to obtain a
												#       reference despite the appearance of using it
												#       as a scalar
		}
		else
		{
			close file;
			return;
		}
	}

	# should never get here
	die "Invalid hex input file $filename\n";
}
