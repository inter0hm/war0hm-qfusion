/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include "../snd_qf/snd_local.h"
#include <SDL.h>

SDL_AudioDeviceID audioid;

void SND_AudioCallback( void *userdata, Uint8 *stream, int len )
{
	float *o = (float *)stream;
	for( int i = 0; i < len/sizeof(float); i++ )
		o[i] = 0.0f;
	// FIXME: call paint buffer
}

/*
 * SNDDMA_Init
 *
 * Try to find a sound device to mix for.
 * Returns false if nothing is found.
 */
bool SNDDMA_Init( void *hwnd, bool verbose )
{
	memset( (void *)&dma, 0, sizeof( dma ) );

	if( s_khz->integer == 48 )
		dma.speed = 48000;
	else if( s_khz->integer == 44 )
		dma.speed = 44100;
	else if( s_khz->integer == 22 )
		dma.speed = 22050;
	else
		dma.speed = 11025;
	dma.msec_per_sample = 1000.0 / dma.speed;

	SDL_AudioSpec desired;
	memset( &desired, 0, sizeof( desired ) );
	desired.callback = SND_AudioCallback;
	desired.channels = 2;
	desired.format = AUDIO_F32;
	desired.freq = dma.speed;
	desired.samples = 0x100;
	desired.size = desired.samples * desired.channels * sizeof( float );
	// We can deal with a frequency change, but let's not try to deal with format change or channels change.
	int allowed_changes = SDL_AUDIO_ALLOW_FREQUENCY_CHANGE;
	SDL_AudioSpec obtained;
	memset( &obtained, 0, sizeof( obtained ) );
	audioid = SDL_OpenAudioDevice( NULL, SDL_FALSE, &desired, &obtained, allowed_changes );**
	dma.speed = obtained.freq;

	s_globalfocus->modified = false;

	dma.channels = 2;
	dma.samplebits = 16;


	if( verbose )
		Com_Printf( "Initializing SDL2 sound subsystem\n" );

	if( verbose )
		Com_Printf( "ok\n" );

	if( verbose )
		Com_Printf( "...completed successfully\n" );

	if( !dsound_init && !wav_init ) {
		Com_Printf( "*** No sound device initialized ***\n" );
		return false;
	}

	return true;
}

/*
 * SNDDMA_GetDMAPos
 *
 * return the current sample position (in mono samples read)
 * inside the recirculating dma buffer, so the mixing code will know
 * how many sample are required to fill it up.
 */
int SNDDMA_GetDMAPos( void )
{
	MMTIME mmtime;
	int s = 0;
	DWORD dwWrite;

	if( dsound_init ) {
		mmtime.wType = TIME_SAMPLES;
		pDSBuf->lpVtbl->GetCurrentPosition( pDSBuf, &mmtime.u.sample, &dwWrite );
		s = mmtime.u.sample - mmstarttime.u.sample;
	} else if( wav_init ) {
		s = snd_sent * WAV_BUFFER_SIZE;
	}

	s >>= sample16;
	s &= ( dma.samples - 1 );

	return s;
}

/*
 * SNDDMA_BeginPainting
 *
 * Makes sure dma.buffer is valid
 */
DWORD locksize;
void SNDDMA_BeginPainting( void )
{
	int reps;
	DWORD dwSize2;
	DWORD *pbuf, *pbuf2;
	HRESULT hresult;
	DWORD dwStatus;

	if( !pDSBuf )
		return;

	// if the buffer was lost or stopped, restore it and/or restart it
	if( pDSBuf->lpVtbl->GetStatus( pDSBuf, &dwStatus ) != DS_OK )
		Com_Printf( "Couldn't get sound buffer status\n" );

	if( dwStatus & DSBSTATUS_BUFFERLOST )
		pDSBuf->lpVtbl->Restore( pDSBuf );

	if( !( dwStatus & DSBSTATUS_PLAYING ) )
		pDSBuf->lpVtbl->Play( pDSBuf, 0, 0, DSBPLAY_LOOPING );

	// lock the dsound buffer

	reps = 0;
	dma.buffer = NULL;

	while( ( hresult = pDSBuf->lpVtbl->Lock( pDSBuf, 0, gSndBufSize, (LPVOID *)&pbuf, &locksize, (LPVOID *)&pbuf2, &dwSize2, 0 ) ) != DS_OK ) {
		if( hresult != DSERR_BUFFERLOST ) {
			Com_Printf( "S_TransferStereo16: Lock failed with error '%s'\n", DSoundError( hresult ) );
			SF_Shutdown( true );
			return;
		} else {
			pDSBuf->lpVtbl->Restore( pDSBuf );
		}

		if( ++reps > 2 )
			return;
	}
	dma.buffer = (unsigned char *)pbuf;
}

/*
 * SNDDMA_Submit
 *
 * Send sound to device if buffer isn't really the dma buffer
 * Also unlocks the dsound buffer
 */
void SNDDMA_Submit( void )
{
	LPWAVEHDR h;
	int wResult;

	if( !dma.buffer )
		return;

	// unlock the dsound buffer
	if( pDSBuf )
		pDSBuf->lpVtbl->Unlock( pDSBuf, dma.buffer, locksize, NULL, 0 );

	if( !wav_init )
		return;

	//
	// find which sound blocks have completed
	//
	while( 1 ) {
		if( snd_completed == snd_sent ) {
			// Com_Printf( "Sound overrun\n" );
			break;
		}

		if( !( lpWaveHdr[snd_completed & WAV_MASK].dwFlags & WHDR_DONE ) ) {
			break;
		}

		snd_completed++; // this buffer has been played
	}

	//
	// submit a few new sound blocks
	//
	while( ( ( snd_sent - snd_completed ) >> sample16 ) < 8 ) {
		h = lpWaveHdr + ( snd_sent & WAV_MASK );
		if( paintedtime / 256 <= snd_sent )
			break;
		snd_sent++;
		/*
		 * Now the data block can be sent to the output device. The
		 * waveOutWrite function returns immediately and waveform
		 * data is sent to the output device in the background.
		 */
		if( s_active ) {
			wResult = waveOutWrite( hWaveOut, h, sizeof( WAVEHDR ) );

			if( wResult != MMSYSERR_NOERROR ) {
				Com_Printf( "Failed to write block to device\n" );
				FreeSound( true );
				return;
			}
		}
	}
}

/*
 * SNDDMA_Shutdown
 *
 * Reset the sound device for exiting
 */
void SNDDMA_Shutdown( bool verbose )
{
	FreeSound( verbose );
}

/*
 * S_Activate
 *
 * Called when the main window gains or loses focus.
 * The window have been destroyed and recreated
 * between a deactivate and an activate.
 */
void S_Activate( bool active )
{
	if( !pDS )
		return;

	if( s_globalfocus->modified ) {
		SNDDMA_Shutdown( false );

		SNDDMA_InitDirect( false );

		s_globalfocus->modified = false;

		if( !pDS )
			return;
	}

	// just set the priority for directsound
	if( pDS->lpVtbl->SetCooperativeLevel( pDS, cl_hwnd, DSSCL_PRIORITY ) != DS_OK ) {
		Com_Printf( "DirectSound SetCooperativeLevel failed\n" );
		SNDDMA_Shutdown( false );
	}
}
