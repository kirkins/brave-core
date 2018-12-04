/* This Source Code Form is subject to the terms of the Mozilla Public
 * License. v. 2.0. If a copy of the MPL was not distributed with this file.
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

// Components
import { Button, Modal, AlertBox } from 'brave-ui'

// Feature-specific components
import {
  Title,
  SubTitle,
  ModalHeader,
  ModalIcon,
  ModalTitle,
  DeviceGrid,
  DeviceContainer,
  DeviceImage,
  ModalSubTitle
} from 'brave-ui/features/sync'

// Modals
import AddNewChainNoCameraModal from './addNewChainNoCamera'
import ScanCodeModal from './scanCode'

// Utils
import { getLocale } from '../../../../common/locale'
import { getDefaultDeviceName } from '../../helpers'

// Images
const syncMobileImg = require('../../../../img/sync/sync_mobile.svg')
const syncDesktopImg = require('../../../../img/sync/sync_desktop.svg')
const syncAddIcon = require('../../../../img/sync/sync_add_icon.svg')

interface Props {
  syncData: Sync.State
  onClose: () => void
  actions: any
}

interface State {
  addNewChainNoCamera: boolean
  scanCode: boolean
}

export default class DeviceTypeModal extends React.PureComponent<Props, State> {
  constructor (props: Props) {
    super(props)
    this.state = {
      addNewChainNoCamera: false,
      scanCode: false
    }
  }

  componentWillMount () {
    // set up sync so we can have access to qr code and code words
    if (this.props.syncData.thisDeviceName === '') {
      this.props.actions.onSetupNewToSync(getDefaultDeviceName())
    }
    this.props.actions.onRequestQRCode()
    this.props.actions.onRequestSyncWords()
  }

  onUserNoticedError = () => {
    this.props.actions.resetSyncSetupError()
  }

  resetSyncTimeout = () => {
    window.setTimeout(() => this.props.actions.onSyncReset(), 5000)
  }

  onClickClose = () => {
    // if there aren't enough devices to sync, reset
    if (this.props.syncData.devices.length < 2) {
      // sync words might take a while to recover from reset amd will not show again
      // for a period so we wait 5s before executing it in case user still wants to sync
      this.resetSyncTimeout as any
    }
    this.props.onClose()
  }

  onClickPhoneTabletButton = () => {
    this.setState({ scanCode: !this.state.scanCode })
    // never call the sync reset if user has regret it
    window.clearTimeout(this.resetSyncTimeout as any)
  }

  onClickComputerButton = () => {
    this.setState({ addNewChainNoCamera: !this.state.addNewChainNoCamera })
    // never call the sync reset if user has regret it
    window.clearTimeout(this.resetSyncTimeout as any)
  }

  render () {
    const { actions, syncData } = this.props
    const { addNewChainNoCamera, scanCode } = this.state

    if (!syncData) {
      return null
    }

    return (
      <Modal id='deviceTypeModal' onClose={this.onClickClose} size='small'>
        {
          syncData.error === 'ERR_SYNC_NO_INTERNET'
          ? <AlertBox okString={getLocale('ok')} onClickOk={this.onUserNoticedError}>
              <Title>{getLocale('errorNoInternetTitle')}</Title>
              <SubTitle>{getLocale('errorNoInternetDescription')}</SubTitle>
            </AlertBox>
          : null
        }
        {
          scanCode
          ? <ScanCodeModal syncData={syncData} actions={actions} onClose={this.onClickPhoneTabletButton} />
          : null
        }
        {
          addNewChainNoCamera
            ? <AddNewChainNoCameraModal syncData={syncData} actions={actions} onClose={this.onClickComputerButton} />
            : null
        }
        <ModalHeader>
          <ModalIcon src={syncAddIcon} />
          <div>
            <ModalTitle level={1}>{getLocale('letsSync')}<br />“{getDefaultDeviceName()}”.</ModalTitle>
            <ModalSubTitle>{getLocale('chooseDeviceType')}</ModalSubTitle>
          </div>
        </ModalHeader>
        <DeviceGrid>
          <DeviceContainer>
            <DeviceImage src={syncMobileImg} />
            <Button
              level='primary'
              type='accent'
              size='medium'
              onClick={this.onClickPhoneTabletButton}
              text={getLocale('phoneTablet')}
            />
          </DeviceContainer>
          <DeviceContainer>
          <DeviceImage src={syncDesktopImg} />
            <Button
              level='primary'
              type='accent'
              size='medium'
              onClick={this.onClickComputerButton}
              text={getLocale('computer')}
            />
          </DeviceContainer>
        </DeviceGrid>
      </Modal>
    )
  }
}
