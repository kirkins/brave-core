/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { bindActionCreators, Dispatch } from 'redux'
import { connect } from 'react-redux'

// Components
import {
  Box,
  DisabledContent,
} from 'brave-ui/features/rewards'
import { Grid, Column, Select, ControlWrapper } from 'brave-ui/components'

// Utils
import { getLocale } from '../../../../common/locale'
import * as rewardsActions from '../actions/rewards_actions'

// Assets
const adsDisabledIcon = require('../../img/ads_disabled.svg')

interface State {
  adsEnabled: boolean
}

interface Props extends Rewards.ComponentProps {
}

class AdsBox extends React.Component<Props, State> {
  constructor(props: Props) {
    super(props)
    this.state = {
      adsEnabled: false
    }
  }

  componentDidMount () {
    const { adsEnabled } = this.props.rewardsData.adsData
    this.setState({ adsEnabled })
  }

  componentWillReceiveProps (nextProps: Props) {
    const { adsEnabled } = nextProps.rewardsData.adsData
    this.setState({ adsEnabled })
  }

  adsDisabled = () => {
    return (
      <DisabledContent
        image={adsDisabledIcon}
        type={'ads'}
      >
        <h3>{getLocale('adsDisabledText')}</h3>
      </DisabledContent>
    )
  }

  onAdsSettingChange = (key: string, value: string) => {
    let newValue: any = value

    if (key === 'adsEnabled') {
      newValue = !this.state.adsEnabled
      this.setState({
        adsEnabled: newValue
      })
    }

    this.props.actions.onAdsSettingSave(key, newValue)
  }

  adsSettings = (enabled?: boolean) => {
    if (!enabled) {
      return null
    }

    const { adsPerHour } = this.props.rewardsData.adsData

    return (
      <Grid columns={1} customStyle={{ maxWidth: '270px', margin: '0 auto' }}>
        <Column size={1} customStyle={{ justifyContent: 'center', flexWrap: 'wrap' }}>
          <ControlWrapper text={getLocale('adsPerHour')}>
            <Select
              value={adsPerHour.toString()}
              onChange={this.onAdsSettingChange.bind(this, 'adsPerHour')}
            >
              {['1', '2', '3', '4', '5'].map((num: string) => {
                return (
                  <div key={`num-per-hour-${num}`} data-value={num}>
                   {getLocale(`adsPerHour${num}`)}
                  </div>
                )
              })}
            </Select>
          </ControlWrapper>
        </Column>
      </Grid>
    )
  }

  render () {
    const { adsData } = this.props.rewardsData

    if (!adsData) {
      return null
    }

    const { adsEnabled } = adsData
    const showEnabled = this.state.adsEnabled || adsEnabled

    return (
      <Box
        title={getLocale('adsTitle')}
        type={'ads'}
        description={getLocale('adsDesc')}
        toggle={true}
        checked={showEnabled}
        settingsChild={this.adsSettings(showEnabled)}
        testId={'braveAdsSettings'}
        disabledContent={this.adsDisabled()}
        onToggle={this.onAdsSettingChange.bind(this, 'adsEnabled', '')}
      />
    )
  }
}

const mapStateToProps = (state: Rewards.ApplicationState) => ({
  rewardsData: state.rewardsData
})

const mapDispatchToProps = (dispatch: Dispatch) => ({
  actions: bindActionCreators(rewardsActions, dispatch)
})

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(AdsBox)
