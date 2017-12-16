 #!/bin/perl

use strict;
use warnings;

my $help = 0;
my $done_popping_opts = 0;
my $download_minidriver_prefix = 1;
my $uart_pkt_type_ind = 0;
my $no_hci_header = 0;
my $outBinFileName;

while(@ARGV && !$done_popping_opts)
{
    if($ARGV[0] eq '-h' || $ARGV[0] eq '--help')
    {
        shift @ARGV;
        $help = 1;
    }
    elsif($ARGV[0] eq '-n' || $ARGV[0] eq '--nodownloadminidriver')
    {
        shift @ARGV;
        $download_minidriver_prefix = 0;
    }
    elsif($ARGV[0] eq '-u' || $ARGV[0] eq '--adduartpkttypeindicators')
    {
        shift @ARGV;
        $uart_pkt_type_ind = 1;
    }
    elsif($ARGV[0] eq '-s' || $ARGV[0] eq '--nohciheader')
    {
        shift @ARGV;
        $no_hci_header = 1;
        $download_minidriver_prefix = 0;
    }
    else
    {
        $done_popping_opts = 1;
    }
}

if(@ARGV != 1)
{
    $help = 1;
}

if($help)
{
    printf( "\n".
            "USAGE: hcd2c.pl [OPTIONS] INFILE\n".
            "       output is to STDOUT.\n".
            "       OPTIONS:\n".
            "       -n | --nodownloadminidriver      ".
                                            "excludes Download_Minidriver\n".
            "       -u | --adduartpkttypeindicators  ".
                                        "adds UART packet type indicators\n".
            "       -s | --nohciheader  ".
                                        "excludes hci header\n".
            "\n" );
    exit -1;
}

open( INFILE, '<', $ARGV[0] ) || die;
binmode INFILE;

## generate BIN file for -s option
if ($no_hci_header)
{
    $outBinFileName  = $ARGV[0].".bin";
    open( OUTFILE, ">$outBinFileName" ) || die;
    binmode OUTFILE;
}

printf("unsigned char ConfigData[] =\n");
printf("{\n");

if($download_minidriver_prefix)
{
    printf("/* Download_Minidriver */\n");
    if($uart_pkt_type_ind)
    {
        printf("0x01, ");
    }
    printf("0x2E, 0xFC, 0x00,\n\n");
}

printf("/* Configuration Data Records (Write_RAM) */\n");

my $done = 0;
my $eor = pack( "CCCCCCC", 0x4E, 0xFC, 0x04, 0xFF, 0xFF, 0xFF, 0xFF );
my $record_prefix = pack( "CC", 0x4C, 0xFC );
while(!$done)
{
    my $header;
    my $result = read( INFILE, $header, 7 );
    ($result == 7) || die;
    if($header eq $eor)
    {
        $done = 1;
    }
    else
    {
        (substr( $header, 0, 2 ) eq $record_prefix) || die;
        my $data_length = ExtractUINT8( $header, 2 ) - 4;
        my $addr = ExtractUINT32LittleEndian( $header, 3 );
        my $data;
        $result = read( INFILE, $data, $data_length );
        ($result == $data_length) || die;
        my $column = 0;
        if($uart_pkt_type_ind)
        {
            printf("0x01, ");
            $column = 6;
        }
        if ($no_hci_header eq 0)
        {
            printf( "0x4C, 0xFC, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X,",
                $data_length+4,
                ($addr)       & 0xFF,
                ($addr >> 8)  & 0xFF,
                ($addr >> 16) & 0xFF,
                ($addr >> 24) & 0xFF );
            $column += 7*6-1;
        }
        my $data_pos = 0;
        while($data_pos < $data_length)
        {
            my $byte_val = unpack( "C", substr( $data, $data_pos, 1 ) );

            if ($no_hci_header)
            {
                my $byte_bin = pack("C", $byte_val);
                print OUTFILE $byte_bin;
            }
            if($column + 6 > 80)
            {
                printf("\n     ");
                $column = 5;
            }
            printf( " 0x%02X,", $byte_val );
            $column += 6;
            $data_pos++;
        }
        printf("\n");
    }
}

printf("\n/* Commit configuration data, reboot firmware (Launch_RAM) */\n");
if($uart_pkt_type_ind)
{
    printf("0x01, ");
}
if ($no_hci_header eq 0)
{
    printf("0x4E, 0xFC, 0x04, 0xFF, 0xFF, 0xFF, 0xFF\n");
}
else
{
    close OUTFILE;
}
printf("};\n");



sub ExtractUINT8
{
    return unpack( "C", substr( $_[0], $_[1], 1 ) );
}

sub ExtractUINT32LittleEndian
{
    return unpack( "L", substr( $_[0], $_[1], 4 ) );
}

